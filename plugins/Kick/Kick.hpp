// Kick.hpp
// Kick drum atom implementation
// AudioSpace (cp.potter@proton.me)

#pragma once

#include "SC_PlugIn.hpp"
#include "../Atom/Atom.hpp"

namespace Atom {

// Kick - A kick drum atom (standalone class, shares AtomData structure)
class Kick : public SCUnit {
public:
    Kick();
    
private:
    void next(int nSamples);
    
    // Shared atom data structure
    AtomData* mData;
    
    // Kick-specific audio generation state
    float mKickPhase;           // Phase for sine wave
    float mKickFreq;            // Current frequency (sweeps from high to low)
    float mKickEnv;             // Envelope amplitude
    float mKickDecayRate;       // Decay rate for envelope
    float mLastTriggerTime;     // Time of last trigger
    float mLastTriggerValue;    // Previous trigger value for edge detection
    float mSampleCounter;       // Sample counter for BPM synchronization
    
    int32 mSpaceId;
    bool mIsRegistered;
};

} // namespace Atom
