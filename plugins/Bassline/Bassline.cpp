// Bassline.cpp
// Bassline atom implementation
// AudioSpace (cp.potter@proton.me)

#include "SC_PlugIn.hpp"
#include "../Atom/Atom.hpp"
#include "Bassline.hpp"
#include <cmath>

static InterfaceTable* ft;

namespace Atom {

// ============================================================================
// Bassline - Bassline atom implementation with frequency modulation
// ============================================================================

Bassline::Bassline() {
    // This sets the unit generator's calculation function pointer to our Bassline::next() method,
    // which will be called for each signal block to generate/process audio.
    // The make_calc_function helper ensures the correct calling convention for SCUnit.
    mCalcFunc = make_calc_function<Bassline, &Bassline::next>();
    
    // Initialize atom data
    mData = new AtomData();
    mData->id = 0;
    mData->position = Vec3(0.0f, 0.0f, 0.0f);
    mData->velocity = Vec3(0.0f, 0.0f, 0.0f);
    mData->collisionRadius = 0.6f;
    mData->parent = nullptr;
    mData->active = true;
    mData->age = 0.0f;
    mData->audioFreq = 110.0f;  // Default bass frequency
    mData->audioAmp = 0.2f;
    
    // Initialize bassline-specific parameters
    mPhase = 0.0f;
    mModPhase = 0.0f;
    mBaseFreq = 110.0f;
    mModFreq = 2.0f;             // Modulation frequency (Hz)
    mModDepth = 0.1f;            // Modulation depth (10% of base frequency)
    mDistortionAmount = 0.3f;    // Distortion amount at peaks
    
    mSpaceId = 0;
    mIsRegistered = false;
    
    // Register with global registry
    auto& registry = AtomRegistry::getInstance();
    mData->id = registry.registerAtom(mData);
    mIsRegistered = true;
    
    next(1);
}

void Bassline::next(int nSamples) {
    float* outbuf = out(0);
    float sr = mWorld->mSampleRate;
    float dt = 1.0f / sr;
    
    // Update parameters from inputs
    // Inputs: x, y, z, vx, vy, vz, radius, freq, amp, bpm, modDepth, distortion
    mData->position = Vec3(in0(0), in0(1), in0(2));
    mData->velocity = Vec3(in0(3), in0(4), in0(5));
    mData->collisionRadius = in0(6) > 0.0f ? in0(6) : 0.6f;
    mData->audioFreq = in0(7) > 0.0f ? in0(7) : 110.0f;
    mData->audioAmp = in0(8);
    
    // Read BPM from input (input 9) - this can be changed dynamically
    // Control rate inputs can be updated in real-time using .set()
    float bpm = numInputs() > 9 ? in0(9) : 120.0f;
    if (bpm <= 0.0f) bpm = 120.0f;  // Safety check
    
    // Update global registry (for sharing between instances)
    auto& registry = AtomRegistry::getInstance();
    registry.setBPM(bpm);
    
    // Use the input BPM value directly (allows dynamic changes)
    // Modulation frequency is synchronized to BPM (modulates once per beat)
    mModFreq = bpm / 60.0f;  // Convert BPM to Hz (beats per second)
    
    // Modulation parameters
    mModDepth = numInputs() > 10 ? in0(10) : 0.1f;
    mDistortionAmount = numInputs() > 11 ? in0(11) : 0.3f;
    
    mBaseFreq = mData->audioFreq;
    
    // Update age
    for (int i = 0; i < nSamples; ++i) {
        mData->age += dt;
    }
    
    // Generate bassline with frequency modulation
    float modPhaseInc = 2.0f * static_cast<float>(M_PI) * mModFreq / sr;
    
    for (int i = 0; i < nSamples; ++i) {
        // Generate modulation signal (sine wave)
        float modSignal = std::sin(mModPhase);
        
        // Modulate frequency - goes in and out of alignment
        // The modulation is synchronized to the waveform phase
        float currentFreq = mBaseFreq * (1.0f + mModDepth * modSignal);
        
        // Calculate phase increment for main oscillator
        float phaseInc = 2.0f * static_cast<float>(M_PI) * currentFreq / sr;
        
        // Generate sine wave
        float sample = std::sin(mPhase);
        
        // Apply distortion at peaks - when the sine wave is near its crest
        // The frequency misalignment causes phase distortion
        float absSample = std::abs(sample);
        if (absSample > 0.7f) {  // Near the peak
            // Calculate how far into the peak we are
            float peakFactor = (absSample - 0.7f) / 0.3f;  // 0 to 1
            
            // Apply distortion based on frequency modulation
            // When frequency is misaligned, we get phase distortion
            float freqDeviation = std::abs(currentFreq - mBaseFreq) / mBaseFreq;
            float distortion = peakFactor * freqDeviation * mDistortionAmount;
            
            // Add harmonics/distortion
            sample += distortion * std::sin(mPhase * 2.0f) * 0.3f;
            sample += distortion * std::sin(mPhase * 3.0f) * 0.15f;
            
            // Soft clipping to prevent harsh distortion
            if (sample > 1.0f) sample = 1.0f - (1.0f - 1.0f / sample) * 0.5f;
            if (sample < -1.0f) sample = -1.0f + (1.0f + 1.0f / sample) * 0.5f;
        }
        
        outbuf[i] = sample * mData->audioAmp;
        
        // Update phases
        mPhase += phaseInc;
        if (mPhase > 2.0f * static_cast<float>(M_PI)) {
            mPhase -= 2.0f * static_cast<float>(M_PI);
        }
        
        mModPhase += modPhaseInc;
        if (mModPhase > 2.0f * static_cast<float>(M_PI)) {
            mModPhase -= 2.0f * static_cast<float>(M_PI);
        }
    }
}

// ============================================================================
// Plugin registration
// ============================================================================

PluginLoad(BasslineUGens) {
    ft = inTable;
    registerUnit<Bassline>(ft, "Bassline", false);
}

} // namespace Atom
