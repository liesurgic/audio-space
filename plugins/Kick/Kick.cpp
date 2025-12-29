// Kick.cpp
// Kick drum atom implementation
// SwampMonster (liesurgic@site.com)

#include "SC_PlugIn.hpp"
#include "../Atom/Atom.hpp"
#include "Kick.hpp"
#include <cmath>

static InterfaceTable* ft;

namespace Atom {

// ============================================================================
// Kick - Kick drum atom implementation
// ============================================================================

Kick::Kick() {
    // This sets the unit generator's calculation function pointer to our Kick::next() method,
    // which will be called for each signal block to generate/process audio.
    // The make_calc_function helper ensures the correct calling convention for SCUnit.
    mCalcFunc = make_calc_function<Kick, &Kick::next>();
    
    // Initialize atom data
    mData = new AtomData();
    mData->id = 0;
    mData->position = Vec3(0.0f, 0.0f, 0.0f);
    mData->velocity = Vec3(0.0f, 0.0f, 0.0f);
    mData->collisionRadius = 0.8f;
    mData->parent = nullptr;
    mData->active = true;
    mData->age = 0.0f;
    mData->audioFreq = 60.0f;
    mData->audioAmp = 0.3f;
    
    // Initialize kick-specific parameters
    mKickPhase = 0.0f;
    mKickFreq = 60.0f;
    mKickEnv = 0.0f;
    mLastTriggerTime = -1.0f;
    mLastTriggerValue = 0.0f;
    mSampleCounter = 0.0f;
    
    // Calculate decay rate for kick envelope
    float kickDecayTime = 0.15f;  // 150ms decay
    float sr = mWorld->mSampleRate;
    mKickDecayRate = std::exp(-1.0f / (kickDecayTime * sr));
    
    mSpaceId = 0;
    mIsRegistered = false;
    
    // Register with global registry
    auto& registry = AtomRegistry::getInstance();
    mData->id = registry.registerAtom(mData);
    mIsRegistered = true;
    
    next(1);
}

void Kick::next(int nSamples) {
    float* outbuf = out(0);
    float sr = mWorld->mSampleRate;
    float dt = 1.0f / sr;
    
    // Update parameters from inputs
    // Inputs: x, y, z, vx, vy, vz, radius, freq, amp, bpm, beatsPerBar
    mData->position = Vec3(in0(0), in0(1), in0(2));
    mData->velocity = Vec3(in0(3), in0(4), in0(5));
    mData->collisionRadius = in0(6) > 0.0f ? in0(6) : 0.8f;
    mData->audioFreq = in0(7) > 0.0f ? in0(7) : 60.0f;
    mData->audioAmp = in0(8);
    
    // Read BPM from input (input 9) - this can be changed dynamically
    // Control rate inputs can be updated in real-time using .set()
    float bpm = numInputs() > 9 ? in0(9) : 120.0f;
    if (bpm <= 0.0f) bpm = 120.0f;  // Safety check
    
    // Update global registry (for sharing between instances)
    auto& registry = AtomRegistry::getInstance();
    registry.setBPM(bpm);
    
    // Use the input BPM value directly (allows dynamic changes)
    float currentBPM = bpm;
    float samplesPerBeat = (sr * 60.0f) / currentBPM;
    
    // Beats per bar (input 10, default 4)
    float beatsPerBar = numInputs() > 10 ? in0(10) : 4.0f;
    float samplesPerBar = samplesPerBeat * beatsPerBar;
    
    // Update age
    for (int i = 0; i < nSamples; ++i) {
        mData->age += dt;
    }
    
    // Auto-trigger based on BPM (every beat)
    bool triggered = false;
    
    for (int i = 0; i < nSamples; ++i) {
        if (mSampleCounter >= samplesPerBeat) {
            mSampleCounter -= samplesPerBeat;
            triggered = true;
            mLastTriggerTime = mData->age;
        }
        mSampleCounter += 1.0f;
    }
    
    // Also check for manual trigger input (input 11)
    float trigger = numInputs() > 11 ? in0(11) : 0.0f;
    if (trigger > 0.0f && mLastTriggerValue <= 0.0f) {
        triggered = true;
        mLastTriggerTime = mData->age;
    }
    mLastTriggerValue = trigger;
    
    // Retrigger on explicit trigger or BPM sync
    if (triggered) {
        mKickEnv = 1.0f;
        mKickFreq = mData->audioFreq * 3.0f;  // Start at 3x the base frequency for pitch sweep
    }
    
    // Generate kick drum sound
    float targetFreq = mData->audioFreq;
    float freqDecayRate = 0.9995f;  // Slow decay of frequency
    
    for (int i = 0; i < nSamples; ++i) {
        // Update envelope
        if (mKickEnv > 0.0f) {
            mKickEnv *= mKickDecayRate;
            if (mKickEnv < 0.0001f) {
                mKickEnv = 0.0f;
            }
        }
        
        // Update frequency sweep (high to low)
        if (mKickFreq > targetFreq) {
            mKickFreq *= freqDecayRate;
            if (mKickFreq < targetFreq) {
                mKickFreq = targetFreq;
            }
        }
        
        // Generate sine wave at current frequency
        float phaseInc = 2.0f * static_cast<float>(M_PI) * mKickFreq / sr;
        float sample = std::sin(mKickPhase) * mKickEnv * mData->audioAmp;
        
        outbuf[i] = sample;
        
        // Update phase
        mKickPhase += phaseInc;
        if (mKickPhase > 2.0f * static_cast<float>(M_PI)) {
            mKickPhase -= 2.0f * static_cast<float>(M_PI);
        }
    }
}

// ============================================================================
// Plugin registration
// ============================================================================

PluginLoad(KickUGens) {
    ft = inTable;
    registerUnit<Kick>(ft, "Kick", false);
}

} // namespace Atom
