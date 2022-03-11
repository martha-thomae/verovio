/////////////////////////////////////////////////////////////////////////////
// Name:        reh.h
// Author:      Klaus Rettinghaus
// Created:     2020
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_REH_H__
#define __VRV_REH_H__

#include "controlelement.h"
#include "textdirinterface.h"
#include "timeinterface.h"

namespace vrv {

class TextElement;

//----------------------------------------------------------------------------
// Reh (rehearsal mark)
//----------------------------------------------------------------------------

/**
 * This class models the MEI <reh> element.
 */
class Reh : public ControlElement,
            public TextDirInterface,
            public TimePointInterface,
            public AttColor,
            public AttLang,
            public AttVerticalGroup {
public:
    /**
     * @name Constructors, destructors, and other standard methods
     * Reset method reset all attribute classes
     */
    ///@{
    Reh();
    virtual ~Reh();
    Object *Clone() const override { return new Reh(*this); }
    void Reset() override;
    std::string GetClassName() const override { return "Reh"; }
    ///@}

    /**
     * @name Getter to interfaces
     */
    ///@{
    TextDirInterface *GetTextDirInterface() override { return dynamic_cast<TextDirInterface *>(this); }
    const TextDirInterface *GetTextDirInterface() const override
    {
        return dynamic_cast<const TextDirInterface *>(this);
    }
    TimePointInterface *GetTimePointInterface() override { return dynamic_cast<TimePointInterface *>(this); }
    const TimePointInterface *GetTimePointInterface() const override
    {
        return dynamic_cast<const TimePointInterface *>(this);
    }
    ///@}

    /**
     * Add an element (text, rend. etc.) to a reh.
     * Only supported elements will be actually added to the child list.
     */
    bool IsSupportedChild(Object *object) override;

    //----------//
    // Functors //
    //----------//

    /**
     * See Object::ResolveRehPosition
     */
    int ResolveRehPosition(FunctorParams *functorParams) override;

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
