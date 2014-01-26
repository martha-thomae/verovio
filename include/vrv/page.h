/////////////////////////////////////////////////////////////////////////////
// Name:        page.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __VRV_PAGE_H__
#define __VRV_PAGE_H__

#include "object.h"
#include "scoredef.h"

namespace vrv {

class DeviceContext;
class Staff;
class System;

    
//----------------------------------------------------------------------------
// Page
//----------------------------------------------------------------------------

/**
 * This class represents a page in a laid-out score (Doc).
 * A Page is contained in a Doc.
 * It contains System objects.
*/
class Page: public DocObject
{
public:
    // constructors and destructors
    Page();
    virtual ~Page();
    
    virtual std::string GetClassName( ) { return "Page"; };
    
    void Clear();
	
	void AddSystem( System *system );
    
	System *GetFirst( );
	System *GetLast( );
	System *GetNext( System *system );
	System *GetPrevious( System *system );
	System *GetAtPos( int y );
	
	int GetSystemCount() const { return (int)m_children.size(); };
    
    /**
     * Return the index position of the page in its document parent
     */
    int GetPageIdx() const { return Object::GetIdx(); };
    
    /**
     * Return the position of the staff on the page, from top to bottom
     */
    int GetStaffPosOnPage( Staff *staff );
    
    /**
     * Do the layout of the page, whichi means aligning is content horizontally
     * and vertically, and justify horizontally and vertically if wanted.
     * This will be done only if m_layoutDone is false or force is true.
     */
    void LayOut( bool force = false );
    
    /**
     * Lay out the content of the page (measures and their content) horizontally
     */
    void LayOutHorizontally( );
    
    /**
     * Justifiy the content of the page (measures and their content) horizontally
     */
    void JustifyHorizontally( );

    /**
     * Lay out the content of the page (system/staves) vertically.
     */
    void LayOutVertically(  );
    
    /**
     * Justifiy the content of the page (system/staves) vertically
     */
    void JustifyVertically( );

    //----------//
    // Functors //
    //----------//
    
    /**
     * Save the content of the page using the FileOutputStream passed as parameter
     * param 0: a pointer to the FileOutputStream.
     */
    virtual int Save( ArrayPtrVoid params );
    
private:
    
public:
    /** Page width (MEI scoredef@page.width). Saved if != -1 */
    int m_pageWidth;
    /** Page height (MEI scoredef@page.height). Saved if != -1 */
    int m_pageHeight;
    /** Page left margin (MEI scoredef@page.leftmar). Saved if != 0 */
    short m_pageLeftMar;
    /** Page right margin (MEI scoredef@page.rightmar). Saved if != 0 */
    short m_pageRightMar;
    /** Page top margin (MEI scoredef@page.topmar). Saved if != 0 */
    short m_pageTopMar;
    /** 
     * Surface (MEI @surface). Saved as facsimile for transciption layout.
     * For now, the target of the <graphic> element within surface is loaded here.
     */
    std::string m_surface;
    
    /**
     * Hold the top scoreDef of the page.
     * The value must be initialized by going through the whole score for finding
     * all the clef or key changes that might occur within the text.
     * The value is initialized with by the Object::SetCurrentScoreDef functor.
     */
    ScoreDef m_drawingScoreDef;

private:
    /**
     * A flag for indicating whether the layout has been done or not
     * If yes, Layout will not layout the page (again) unless
     * the force parameter is set.
     */
    bool m_layoutDone;
    
};

} // namespace vrv

#endif
