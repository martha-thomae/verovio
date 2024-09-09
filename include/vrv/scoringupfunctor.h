/////////////////////////////////////////////////////////////////////////////
// Name:        scoringupfunctor.h
// Author:      Martha E. M. Thomae Elias
// Created:     2024
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
///
#ifndef __VRV_SCORINGUPFUNCTOR_H__
#define __VRV_SCORINGUPFUNCTOR_H__

#include "functor.h"

namespace vrv {

//----------------------------------------------------------------------------
// ScoringUpFunctor
//----------------------------------------------------------------------------

/**
 * This class...
 */
class ScoringUpFunctor : public Functor {
public:
    /**
     * @name Constructors, destructors
     */
    ///@{
    ScoringUpFunctor();
    virtual ~ScoringUpFunctor() = default;
    ///@}
    
    /*
     * Abstract base implementation
     */
    bool ImplementsEndInterface() const override { return true; }

    /*
     * Methods
     */
    /**
     * @name: Divide the notes of a voice into sequences to be processed individualy
     */
    ///@{
    std::vector<std::vector<std::pair<LayerElement*, data_DURATION>>> SubdivideIntoBoundedSequences(std::vector<std::pair<LayerElement*, data_DURATION>> dursInVoiceSameMensur);
    ///@}
    /**
     * @name: Find @dur.quality of notes (perfecta / imperfecta / altera)
     */
    ///@{
    //void FindDurQuals(std::vector<std::vector<std::pair<LayerElement*, data_DURATION>>> listOfSequences);
    void ProcessBoundedSequences(std::vector<std::vector<std::pair<LayerElement*, data_DURATION>>> listOfSequences);
    std::vector<std::pair<LayerElement*, data_DURATION>> GetBoundedNotes(std::vector<std::pair<LayerElement*, data_DURATION>> sequence);
    std::pair<LayerElement*, data_DURATION> FindDotOfDivision(std::vector<std::pair<LayerElement*, data_DURATION>> middleSeq);
    double GetValueInMinims(std::vector<std::pair<LayerElement*, data_DURATION>> middleSeq);
    double GetValueInUnit(double valueInMinims);
    //void FindDurQuals(std::vector<std::pair<LayerElement*, data_DURATION>> sequence);
    void FindDurQuals(std::vector<std::pair<LayerElement*, data_DURATION>> sequence, int valueInSemibreves);
    ///@}
    /**
    * @name: Find the duration value of the note in minims
    */
    ///@{
    double GetDurNumberValue(std::pair<LayerElement*, data_DURATION> elementDurPair, bool followedByDot, LayerElement* nextElement);
    ///@}
    /**
     * @name Apply principles of imperfection and alteration
     */
    ///@{
    bool ImperfectionAPP(std::vector<std::pair<LayerElement*, data_DURATION>> sequence);
    bool ImperfectionAPA(std::vector<std::pair<LayerElement*, data_DURATION>> sequence);
    bool Alteration(std::vector<std::pair<LayerElement*, data_DURATION>> sequence);
    bool LeavePerfect(std::vector<std::pair<LayerElement*, data_DURATION>> sequence);
    ///@}
    
    /*
     * Functor interface
     */
    ///@{
    FunctorCode VisitLayer(Layer *layer) override;
    FunctorCode VisitLayerElement(LayerElement *layerElement) override;
    ///@}
    ///

protected:
    //
private:
    //
public:
    std::map<std::string, int> mensuration;
    std::vector<std::pair<LayerElement*, data_DURATION>> dursInVoiceSameMensur = {};
    std::vector<std::vector<std::pair<LayerElement*, data_DURATION>>> listOfSequences;
private:
    // The current score time in the measure (incremented by each element)
    double m_currentScoreTime;
    // The current Mensur
    Mensur *m_currentMensur;
    // The current notation type
    data_NOTATIONTYPE m_notationType;
};

} // namespace vrv

#endif /* scoringupfunctor_h */
