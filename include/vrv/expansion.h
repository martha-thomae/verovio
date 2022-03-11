/////////////////////////////////////////////////////////////////////////////
// Name:        expansion.h
// Author:      Klaus Rettinghaus
// Created:     22/02/2017
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_EXPANSION_H__
#define __VRV_EXPANSION_H__

#include "atts_shared.h"
#include "plistinterface.h"
#include "systemelement.h"

namespace vrv {

class Expansion;

//----------------------------------------------------------------------------
// Expansion
//----------------------------------------------------------------------------

/**
 * This class represents a MEI expansion.
 */
class Expansion : public SystemElement, public PlistInterface {
public:
    /**
     * @name Constructors, destructors, and other standard methods
     * Reset method resets all attribute classes
     */
    ///@{
    Expansion();
    virtual ~Expansion();
    Object *Clone() const override { return new Expansion(*this); }
    void Reset() override;
    std::string GetClassName() const override { return "Expansion"; }
    ///@}

    /**
     * @name Getter to interfaces
     */
    ///@{
    PlistInterface *GetPlistInterface() override { return dynamic_cast<PlistInterface *>(this); }
    const PlistInterface *GetPlistInterface() const override { return dynamic_cast<const PlistInterface *>(this); }
    ////@}

    //----------//
    // Functors //
    //----------//

protected:
    //
private:
    //
public:
    //
private:
    //
};

} // namespace vrv

#endif
