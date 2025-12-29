// Bassline.hpp
// Bassline atom implementation
// AudioSpace (cp.potter@proton.me)

#pragma once

#include "SC_PlugIn.hpp"
#include "../Atom/Atom.hpp"

namespace Atom {

// Bassline - A bassline atom with frequency modulation causing peak distortion
class Bassline : public SCUnit {
public:
    Bassline();
    
private:
    void next(int nSamples);
    
    // Shared atom data structure
    AtomData* mData;
    
    // Bassline-specific audio generation state
    float mPhase;                // Phase for sine wave
    float mModPhase;             // Phase for frequency modulation
    float mBaseFreq;             // Base frequency
    float mModFreq;              // Modulation frequency
    float mModDepth;             // Modulation depth
    float mDistortionAmount;     // Amount of distortion at peaks
    
    int32 mSpaceId;
    bool mIsRegistered;
};

} // namespace Atom
