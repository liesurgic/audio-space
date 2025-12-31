// Womp.hpp
// Womp atom implementation - combines Kick and Bassline
// AudioSpace (cp.potter@proton.me)

#pragma once

#include "SC_PlugIn.hpp"
#include "../Atom/Atom.hpp"

namespace Atom {

// Womp - Combines kick and bassline, synchronizing bassline peaks with kick triggers
class Womp : public SCUnit {
public:
    Womp();
    
private:
    void next(int nSamples);
    
    // Shared atom data structure
    AtomData* mData;
    
    // Kick state
    float mKickPhase;
    float mKickFreq;
    float mKickEnv;
    float mKickDecayRate;
    float mSampleCounter;
    
    // Bassline state
    float mBassPhase;
    float mModPhase;
    float mBaseFreq;
    float mModFreq;
    float mModDepth;
    float mDistortionAmount;
    
    // Sidechain compression state
    float mCompressorEnv;       // Compression envelope follower
    float mCompressorAttack;    // Attack time constant
    float mCompressorRelease;   // Release time constant
    
    int32 mSpaceId;
    bool mIsRegistered;
};

} // namespace Atom
