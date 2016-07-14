/////////////////////////////////////////////////////////////////////////////
// Name:        scoredef.cpp
// Author:      Laurent Pugin
// Created:     2013/11/08
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "scoredef.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "clef.h"
#include "keysig.h"
#include "mensur.h"
#include "metersig.h"
#include "system.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// ScoreDefElement
//----------------------------------------------------------------------------

ScoreDefElement::ScoreDefElement(std::string classid) : Object(classid), ScoreDefInterface()
{
    RegisterInterface(ScoreDefInterface::GetAttClasses(), ScoreDefInterface::IsInterface());

    Reset();
}

ScoreDefElement::~ScoreDefElement()
{
}

void ScoreDefElement::Reset()
{
    Object::Reset();
    ScoreDefInterface::Reset();
}

bool ScoreDefElement::HasClefInfo() const
{
    if (this->HasClefAttrInfo()) return true;
    return (this->HasClefElementInfo());
}

bool ScoreDefElement::HasKeySigInfo() const
{
    if (this->HasKeySigAttrInfo()) return true;
    return (this->HasKeySigElementInfo());
}

bool ScoreDefElement::HasMensurInfo() const
{
    if (this->HasMensurAttrInfo()) return true;
    return (this->HasMensurElementInfo());
}

bool ScoreDefElement::HasMeterSigInfo() const
{
    if (this->HasMeterSigAttrInfo()) return true;
    return (this->HasMeterSigElementInfo());
}

bool ScoreDefElement::HasClefAttrInfo() const
{
    // We need at least a @clef.shape and a @clef.line ?
    return (this->HasClefShape() && this->HasClefLine());

    // Eventually we can look for a child clef element
    // We would probably need to take into account app and rdg?
    return false;
}

bool ScoreDefElement::HasKeySigAttrInfo() const
{
    return (this->HasKeySig());
}

bool ScoreDefElement::HasMensurAttrInfo() const
{
    // What is the minimum we need? Checking only some for now. Need clarification
    return (this->HasProlatio() || this->HasTempus() || this->HasProportNum() || this->HasProportNumbase());
}

bool ScoreDefElement::HasMeterSigAttrInfo() const
{
    return (this->HasMeterCount() || this->HasMeterSym() || this->HasMeterUnit());
}

bool ScoreDefElement::HasClefElementInfo() const
{
    // Eventually we can look for a child clef element
    // We would probably need to take into account app and rdg?
    return false;
}

bool ScoreDefElement::HasKeySigElementInfo() const
{
    return false;
}

bool ScoreDefElement::HasMensurElementInfo() const
{
    return false;
}

bool ScoreDefElement::HasMeterSigElementInfo() const
{
    return false;
}

Clef *ScoreDefElement::GetClefCopy() const
{
    Clef *copy = NULL;
    if (this->HasClefAttrInfo()) {
        copy = new Clef(this);
    }
    else if (this->HasClefElementInfo()) {
        // Eventually return a copy of the child element;
    }
    // Always check if HasClefInfo() is true before asking for a copy
    assert(copy);
    copy->SetScoreOrStaffDefAttr(true);
    return copy;
}

KeySig *ScoreDefElement::GetKeySigCopy() const
{
    KeySig *copy = NULL;
    if (this->HasKeySigAttrInfo()) {
        copy = new KeySig(this);
    }
    else {
        // Eventually return a copy of the child element;
    }
    // Always check if HasKeySigInfo() is true before asking for a copy
    assert(copy);
    copy->SetScoreOrStaffDefAttr(true);
    return copy;
}

Mensur *ScoreDefElement::GetMensurCopy() const
{
    Mensur *copy = NULL;
    if (this->HasMensurAttrInfo()) {
        copy = new Mensur(this);
    }
    else {
        // Eventually return a copy of the child element;
    }
    // Always check if HasMensurInfo() is true before asking for a copy
    assert(copy);
    copy->SetScoreOrStaffDefAttr(true);
    return copy;
}

MeterSig *ScoreDefElement::GetMeterSigCopy() const
{
    MeterSig *copy = NULL;
    if (this->HasMeterSigAttrInfo()) {
        copy = new MeterSig(this);
    }
    else {
        // Eventually return a copy of the child element;
    }
    // Always check if HasMeterSigInfo() is true before asking for a copy
    assert(copy);
    copy->SetScoreOrStaffDefAttr(true);
    return copy;
}

//----------------------------------------------------------------------------
// ScoreDef
//----------------------------------------------------------------------------

ScoreDef::ScoreDef() : ScoreDefElement("scoredef-"), ObjectListInterface()
{
    Reset();
}

ScoreDef::~ScoreDef()
{
}

void ScoreDef::Reset()
{
    ScoreDefElement::Reset();

    m_drawLabels = false;
    m_drawingWidth = 0;
    m_setAsDrawing = false;
}

void ScoreDef::AddStaffGrp(StaffGrp *staffGrp)
{
    assert(m_children.empty());
    staffGrp->SetParent(this);
    m_children.push_back(staffGrp);
    Modify();
}

void ScoreDef::ReplaceDrawingValues(ScoreDef *newScoreDef)
{
    assert(newScoreDef);

    m_setAsDrawing = true;

    bool drawClef = false;
    bool drawKeySig = false;
    bool drawMensur = false;
    bool drawMeterSig = false;
    Clef *clef = NULL;
    KeySig *keySig = NULL;
    Mensur *mensur = NULL;
    MeterSig *meterSig = NULL;

    if (newScoreDef->HasClefInfo()) {
        drawClef = true;
        clef = newScoreDef->GetClefCopy();
    }
    if (newScoreDef->HasKeySigInfo()) {
        drawKeySig = true;
        keySig = newScoreDef->GetKeySigCopy();
    }
    if (newScoreDef->HasMensurInfo()) {
        drawMensur = true;
        mensur = newScoreDef->GetMensurCopy();
    }
    if (newScoreDef->HasMeterSigInfo()) {
        drawMeterSig = true;
        meterSig = newScoreDef->GetMeterSigCopy();
    }

    ArrayPtrVoid params;
    params.push_back(clef);
    params.push_back(keySig);
    params.push_back(mensur);
    params.push_back(meterSig);
    Functor replaceDrawingValuesInScoreDef(&Object::ReplaceDrawingValuesInStaffDef);
    this->Process(&replaceDrawingValuesInScoreDef, &params);

    if (clef) delete clef;
    if (keySig) delete keySig;
    if (mensur) delete mensur;
    if (meterSig) delete meterSig;

    // The keySig cancellation flag is the same as keySig because we draw cancellation with new key sig
    this->SetRedrawFlags(drawClef, drawKeySig, drawMensur, drawMeterSig, drawKeySig, false);
}

void ScoreDef::ReplaceDrawingValues(StaffDef *newStaffDef)
{
    assert(newStaffDef);

    // first find the staffDef with the same @n
    StaffDef *staffDef = this->GetStaffDef(newStaffDef->GetN());

    // if found, replace attributes
    if (staffDef) {
        if (newStaffDef->HasClefInfo()) {
            staffDef->SetDrawClef(true);
            Clef *clef = newStaffDef->GetClefCopy();
            staffDef->SetCurrentClef(clef);
            delete clef;
        }
        if (newStaffDef->HasKeySigInfo()) {
            staffDef->SetDrawKeySig(true);
            staffDef->SetDrawKeySigCancellation(true);
            KeySig *keySig = newStaffDef->GetKeySigCopy();
            staffDef->SetCurrentKeySig(keySig);
            delete keySig;
        }
        if (newStaffDef->HasMensurInfo()) {
            staffDef->SetDrawMensur(true);
            Mensur *mensur = newStaffDef->GetMensurCopy();
            staffDef->SetCurrentMensur(mensur);
            delete mensur;
        }
        if (newStaffDef->HasMeterSigInfo()) {
            staffDef->SetDrawMeterSig(true);
            MeterSig *meterSig = newStaffDef->GetMeterSigCopy();
            staffDef->SetCurrentMeterSig(meterSig);
            delete meterSig;
        }
        // copy other attributes if present
        if (newStaffDef->HasLabel()) staffDef->SetLabel(newStaffDef->GetLabel());
        if (newStaffDef->HasLabelAbbr()) staffDef->SetLabelAbbr(newStaffDef->GetLabelAbbr());
    }
    else {
        LogWarning("StaffDef with xml:id '%s' could not be found", newStaffDef->GetUuid().c_str());
    }
}

void ScoreDef::FilterList(ListOfObjects *childList)
{
    // We want to keep only staffDef
    ListOfObjects::iterator iter = childList->begin();

    while (iter != childList->end()) {
        if ((*iter)->Is() != STAFFDEF) {
            iter = childList->erase(iter);
        }
        else {
            iter++;
        }
    }
}

StaffDef *ScoreDef::GetStaffDef(int n)
{
    StaffDef *staffDef = NULL;

    this->ResetList(this);
    ListOfObjects *childList = this->GetList(this);
    ListOfObjects::iterator iter;

    for (iter = childList->begin(); iter != childList->end(); ++iter) {
        if ((*iter)->Is() != STAFFDEF) continue;
        staffDef = dynamic_cast<StaffDef *>(*iter);
        assert(staffDef);
        if (staffDef->GetN() == n) {
            return staffDef;
        }
    }

    return staffDef;
}

void ScoreDef::SetRedrawFlags(
    bool clef, bool keySig, bool mensur, bool meterSig, bool keySigCancellation, bool applyToAll)
{
    m_setAsDrawing = true;

    ArrayPtrVoid params;
    params.push_back(&clef);
    params.push_back(&keySig);
    params.push_back(&mensur);
    params.push_back(&meterSig);
    params.push_back(&keySigCancellation);
    params.push_back(&applyToAll);
    Functor setStaffDefDraw(&Object::SetStaffDefRedrawFlags);
    this->Process(&setStaffDefDraw, &params);
}

void ScoreDef::SetDrawingWidth(int drawingWidth)
{
    m_drawingWidth = drawingWidth;
}

//----------------------------------------------------------------------------
// StaffGrp
//----------------------------------------------------------------------------

StaffGrp::StaffGrp()
    : Object()
    , ObjectListInterface()
    , AttCommon()
    , AttCommonPart()
    , AttLabelsAddl()
    , AttStaffgroupingsym()
    , AttStaffGrpVis()
{
    RegisterAttClass(ATT_COMMON);
    RegisterAttClass(ATT_COMMONPART);
    RegisterAttClass(ATT_LABELSADDL);
    RegisterAttClass(ATT_STAFFGROUPINGSYM);
    RegisterAttClass(ATT_STAFFGRPVIS);

    Reset();
}

StaffGrp::~StaffGrp()
{
}

void StaffGrp::Reset()
{
    Object::Reset();
    ResetCommon();
    ResetCommonPart();
    ResetLabelsAddl();
    ResetStaffgroupingsym();
    ResetStaffGrpVis();
}

void StaffGrp::AddStaffDef(StaffDef *staffDef)
{
    staffDef->SetParent(this);
    m_children.push_back(staffDef);
    Modify();
}

void StaffGrp::AddStaffGrp(StaffGrp *staffGrp)
{
    staffGrp->SetParent(this);
    m_children.push_back(staffGrp);
    Modify();
}

void StaffGrp::FilterList(ListOfObjects *childList)
{
    // We want to keep only staffDef
    ListOfObjects::iterator iter = childList->begin();

    while (iter != childList->end()) {
        if ((*iter)->Is() != STAFFDEF) {
            iter = childList->erase(iter);
        }
        else {
            iter++;
        }
    }
}

//----------------------------------------------------------------------------
// StaffDef
//----------------------------------------------------------------------------

StaffDef::StaffDef()
    : ScoreDefElement("staffdef-")
    , AttCommon()
    , AttCommonPart()
    , AttLabelsAddl()
    , AttNotationtype()
    , AttScalable()
    , AttStaffDefVis()
    , AttTransposition()
{
    RegisterAttClass(ATT_COMMON);
    RegisterAttClass(ATT_COMMONPART);
    RegisterAttClass(ATT_LABELSADDL);
    RegisterAttClass(ATT_NOTATIONTYPE);
    RegisterAttClass(ATT_SCALABLE);
    RegisterAttClass(ATT_STAFFDEFVIS);
    RegisterAttClass(ATT_TRANSPOSITION);

    Reset();
}

StaffDef::~StaffDef()
{
}

void StaffDef::Reset()
{
    ScoreDefElement::Reset();
    StaffDefDrawingInterface::Reset();
    ResetCommon();
    ResetCommonPart();
    ResetLabelsAddl();
    ResetNotationtype();
    ResetScalable();
    ResetStaffDefVis();
    ResetTransposition();
}

//----------------------------------------------------------------------------
// ScoreDef functor methods
//----------------------------------------------------------------------------

int ScoreDef::CastOffSystems(ArrayPtrVoid *params)
{
    // param 0: a pointer to the system we are taking the content from
    // param 1: a pointer the page we are adding system to (unused)
    // param 2: a pointer to the current system
    // param 3: the cummulated shift (m_drawingXRel of the first measure of the current system) (unused)
    // param 4: the system width (unused)
    // param 5: the current scoreDef width
    System *contentSystem = static_cast<System *>((*params).at(0));
    System **currentSystem = static_cast<System **>((*params).at(2));
    int *currentScoreDefWidth = static_cast<int *>((*params).at(5));

    // Since the functor returns FUNCTOR_SIBLINGS we should never go lower than the system children
    assert(dynamic_cast<System *>(this->m_parent));

    // Special case where we use the Relinquish method.
    // We want to move the measure to the currentSystem. However, we cannot use DetachChild
    // from the content System because this screws up the iterator. Relinquish gives up
    // the ownership of the Measure - the contentSystem will be deleted afterwards.
    ScoreDef *scoreDef = dynamic_cast<ScoreDef *>(contentSystem->Relinquish(this->GetIdx()));
    (*currentSystem)->AddScoreDef(scoreDef);
    // This is not perfect since now the scoreDefWith is the one of the intermediate scoreDefs (and not
    // the initial one - for this to be corrected, we would need two parameters, one for the current initial
    // scoreDef and one for the current that will be the initial one at the next system
    // Also, the abbr label (width) changes would not be taken into account
    (*currentScoreDefWidth) = this->GetDrawingWidth() + contentSystem->GetDrawingAbbrLabelsWidth();
    ;

    return FUNCTOR_SIBLINGS;
}

//----------------------------------------------------------------------------
// StaffDef functor methods
//----------------------------------------------------------------------------

int StaffDef::ReplaceDrawingValuesInStaffDef(ArrayPtrVoid *params)
{
    // param 0: Clef pointer (NULL if none)
    // param 1: KeySig pointer (NULL if none)
    // param 2: Mensur pointer (NULL if none)
    // param 3: MeterSig pointer (NULL if none)
    Clef *clef = static_cast<Clef *>((*params).at(0));
    KeySig *keySig = static_cast<KeySig *>((*params).at(1));
    Mensur *mensur = static_cast<Mensur *>((*params).at(2));
    MeterSig *meterSig = static_cast<MeterSig *>((*params).at(3));

    if (clef) {
        this->SetCurrentClef(clef);
    }
    if (keySig) {
        this->SetCurrentKeySig(keySig);
    }
    if (mensur) {
        this->SetCurrentMensur(mensur);
    }
    if (meterSig) {
        this->SetCurrentMeterSig(meterSig);
    }

    return FUNCTOR_CONTINUE;
}

int StaffDef::SetStaffDefRedrawFlags(ArrayPtrVoid *params)
{
    // param 0: bool clef flag
    // param 1: bool keysig flag
    // param 2: bool mensur flag
    // param 3: bool meterSig flag
    // param 4: bool keySig cancellation flag
    // param 5: bool the flag for indicating if apply to all or not
    bool *clef = static_cast<bool *>((*params).at(0));
    bool *keysig = static_cast<bool *>((*params).at(1));
    bool *mensur = static_cast<bool *>((*params).at(2));
    bool *meterSig = static_cast<bool *>((*params).at(3));
    bool *keySigCancellation = static_cast<bool *>((*params).at(4));
    bool *applyToAll = static_cast<bool *>((*params).at(5));

    if ((*clef) || (*applyToAll)) {
        this->SetDrawClef((*clef));
    }
    if ((*keysig) || (*applyToAll)) {
        this->SetDrawKeySig((*keysig));
    }
    if ((*mensur) || (*applyToAll)) {
        this->SetDrawMensur((*mensur));
    }
    if ((*meterSig) || (*applyToAll)) {
        this->SetDrawMeterSig((*meterSig));
    }
    if ((*keySigCancellation) || (*applyToAll)) {
        this->SetDrawKeySigCancellation((*keySigCancellation));
    }

    return FUNCTOR_CONTINUE;
}

} // namespace vrv
