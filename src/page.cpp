/////////////////////////////////////////////////////////////////////////////
// Name:        page.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "page.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "aligner.h"
#include "bboxdevicecontext.h"
#include "vrvdef.h"
#include "doc.h"
#include "view.h"
#include "staff.h"
#include "system.h"

namespace vrv {

//----------------------------------------------------------------------------
// Page
//----------------------------------------------------------------------------

Page::Page() :
	DocObject("page-")
{
	Clear( );
}

Page::~Page()
{
}

void Page::Clear( )
{
	ClearChildren( );
    m_drawingScoreDef.Clear();
    m_layoutDone = false;
    // by default we have no values and use the document ones
    m_pageHeight = -1;
    m_pageWidth = -1;
    m_pageLeftMar = 0;
    m_pageRightMar = 0;
    m_pageTopMar = 0;
    this->ResetUuid();
}


int Page::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    FileOutputStream *output = static_cast<FileOutputStream*>(params[0]);         
    if (!output->WritePage( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
}


void Page::AddSystem( System *system )
{
	system->SetParent( this );
	m_children.push_back( system );
    Modify();
}

int Page::GetStaffPosOnPage( Staff *staff )
{
    /*
    int position = -1;
    bool success = false;
    ArrayPtrVoid params;
    params.Add( staff );
    params.Add( &position );
    params.Add( &success );
    MusStaffFunctor getStaffPosOnPage( &Staff::GetPosOnPage );
    Process( &getStaffPosOnPage, params );    
    return position;
    */ // ax2.3
    return 0;
}


System *Page::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return dynamic_cast<System*>(m_children[0]);
}

System *Page::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = GetSystemCount() - 1;
	return dynamic_cast<System*>(m_children[i]);
}

System *Page::GetNext( System *system )
{
    if ( !system || m_children.empty())
        return NULL;
        
	int i = this->GetChildIndex( system );

	if ((i == -1 ) || ( i >= GetSystemCount() - 1 ))
		return NULL;
	
	return dynamic_cast<System*>(m_children[i + 1]);
}

System *Page::GetPrevious( System *system  )
{
    if ( !system || m_children.empty())
        return NULL;
        
	int i = GetChildIndex( system );

	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return dynamic_cast<System*>(m_children[i - 1]);
}


System *Page::GetAtPos( int y )
{

	//y -= ( SYSTEM_OFFSET / 2 ); // SYSTEM_OFFSET used to be 190
    System *system = this->GetFirst();
	if ( !system )
		return NULL;
	
    System *next = NULL;
	while ( (next = this->GetNext(system)) )
	{
		if ( (int)next->m_yAbs < y )
		{
			return system;
		}
		system = next;
	}

	return system;
}

void Page::LayOut( bool force )
{
    if ( m_layoutDone && ! force ) {
        return;
    }
    
    this->LayOutHorizontally();
    this->LayOutVertically();
    //this->JustifyHorizontally();
    
    m_layoutDone = true;
}
    
void Page::LayOutHorizontally( )
{
    if (!dynamic_cast<Doc*>(m_parent)) {
        assert( false );
        return;
    }
    Doc *doc = dynamic_cast<Doc*>(m_parent);
    
    // Doc::SetDrawingPage should have been called before
    // Make sure we have the correct page
    assert( this == doc->GetDrawingPage() );
    
    ArrayPtrVoid params;
    
    // Align the content of the page using measure aligners
    // After this:
    // - each LayerElement object will have its Alignment pointer initialized
    MeasureAligner *measureAlignerPtr = NULL;
    double time = 0.0;
    params.push_back( &measureAlignerPtr );
    params.push_back( &time );
    MusFunctor alignHorizontally( &Object::AlignHorizontally );
    this->Process( &alignHorizontally, params );
    
    // Set the X position of each Alignment
    // Does a duration-based non linear spacing looking at the duration space between two Alignment objects
    params.clear();
    double previousTime = 0.0;
    int previousXRel = 0;
    params.push_back( &previousTime );
    params.push_back( &previousXRel );
    MusFunctor setAlignmentX( &Object::SetAligmentXPos );
    // Special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &setAlignmentX );
    this->Process( &setAlignmentX, params );
    
    // Render it for filling the bounding boxing
    View view;
    BBoxDeviceContext bb_dc( &view, 0, 0 );
    view.SetDoc(doc);
    // Do not do the layout in this view - otherwise we will loop...
    view.SetPage( this->GetIdx(), false );
    view.DrawCurrentPage(  &bb_dc, false );
    
    // Adjust the X shift of the Alignment looking at the bounding boxes
    // Look at each LayerElement and changes the m_xShift if the bouding box is overlapping
    params.clear();
    int min_pos = 0;
    int measure_width = 0;
    params.push_back( &min_pos );
    params.push_back( &measure_width );
    MusFunctor setBoundingBoxXShift( &Object::SetBoundingBoxXShift );
    MusFunctor setBoundingBoxXShiftEnd( &Object::SetBoundingBoxXShiftEnd );
    this->Process( &setBoundingBoxXShift, params, &setBoundingBoxXShiftEnd );
    
    // Integrate the X bounding box shift of the elements
    // Once the m_xShift have been calculated, move all positions accordingly
    params.clear();
    int shift = 0;
    params.push_back( &shift );
    MusFunctor integrateBoundingBoxXShift( &Object::IntegrateBoundingBoxXShift );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &integrateBoundingBoxXShift );
    this->Process( &integrateBoundingBoxXShift, params );
    
    // Adjust measure X position
    params.clear();
    shift = 0;
    params.push_back( &shift );
    MusFunctor alignMeasures( &Object::AlignMeasures );
    MusFunctor alignMeasuresEnd( &Object::AlignMeasuresEnd );
    this->Process( &alignMeasures, params, &alignMeasuresEnd );
}
    
void Page::LayOutVertically( )
{
    if (!dynamic_cast<Doc*>(m_parent)) {
        assert( false );
        return;
    }
    Doc *doc = dynamic_cast<Doc*>(m_parent);
    
    // Doc::SetDrawingPage should have been called before
    // Make sure we have the correct page
    assert( this == doc->GetDrawingPage() );
    
    ArrayPtrVoid params;
    
    // Align the content of the page using system aligners
    // After this:
    // - each Staff object will then have its StaffAlignment pointer initialized
    SystemAligner *systemAlignerPtr = NULL;
    int staffNb = 0;
    params.push_back( &systemAlignerPtr );
    params.push_back( &staffNb );
    MusFunctor alignVertically( &Object::AlignVertically );
    this->Process( &alignVertically, params );
    
    // Render it for filling the bounding boxing
    View view;
    BBoxDeviceContext bb_dc( &view, 0, 0 );
    view.SetDoc(doc);
    // Do not do the layout in this view - otherwise we will loop...
    view.SetPage( this->GetIdx(), false );
    view.DrawCurrentPage(  &bb_dc, false );
    
    // Adjust the Y shift of the StaffAlignment looking at the bounding boxes
    // Look at each Staff and changes the m_yShift if the bounding box is overlapping
    params.clear();
    int previous_height = 0;
    int system_height = 0;
    params.push_back( &previous_height );
    params.push_back( &system_height );
    MusFunctor setBoundingBoxYShift( &Object::SetBoundingBoxYShift );
    MusFunctor setBoundingBoxYShiftEnd( &Object::SetBoundingBoxYShiftEnd );
    this->Process( &setBoundingBoxYShift, params, &setBoundingBoxYShiftEnd );
    
    // Set the Y position of each StaffAlignment
    // Adjusts the Y shift for making sure there is a minimal space (staffMargin) between each staff
    params.clear();
    int previousStaffHeight = 0; // 0 for the first staff, reset for each system (see System::SetAlignmentYPos)
    int staffMargin = 1.0 * doc->m_drawingStaffSize[0]; // the minimal space we want to have between each staff
    int* interlineSizes = doc->m_drawingInterl; // the interline sizes to be used for calculating the (previous) staff height
    params.push_back( &previousStaffHeight );
    params.push_back( &staffMargin );
    params.push_back( &interlineSizes );
    MusFunctor setAlignmentY( &Object::SetAligmentYPos );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &setAlignmentY );
    this->Process( &setAlignmentY, params );
    
    // Integrate the Y shift of the staves
    // Once the m_yShift have been calculated, move all positions accordingly
    params.clear();
    int shift = 0;
    params.push_back( &shift );
    MusFunctor integrateBoundingBoxYShift( &Object::IntegrateBoundingBoxYShift );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &integrateBoundingBoxYShift );
    this->Process( &integrateBoundingBoxYShift, params );
    
    // Adjust system Y position
    params.clear();
    shift = doc->m_drawingPageHeight - doc->m_drawingPageTopMar;
    int systemMargin = 0;// doc->m_drawingStaffSize[0];
    params.push_back( &shift );
    params.push_back( &systemMargin );
    MusFunctor alignSystems( &Object::AlignSystems );
    MusFunctor alignSystemsEnd( &Object::AlignSystemsEnd );
    this->Process( &alignSystems, params, &alignSystemsEnd );
}
    
void Page::JustifyHorizontally( )
{
    if (!dynamic_cast<Doc*>(m_parent)) {
        assert( false );
        return;
    }
    Doc *doc = dynamic_cast<Doc*>(m_parent);
    
    // Doc::SetDrawingPage should have been called before
    // Make sure we have the correct page
    assert( this == doc->GetDrawingPage() );
    
    ArrayPtrVoid params;
    
    // Justify X position
    params.clear();
    double ratio = 0.0;
    int systemFullWidth = doc->m_drawingPageWidth - doc->m_drawingPageLeftMar - doc->m_drawingPageRightMar;
    params.push_back( &ratio );
    params.push_back( &systemFullWidth );
    MusFunctor justifyX( &Object::JustifyX );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &justifyX );
    this->Process( &justifyX, params );
}

} // namespace vrv