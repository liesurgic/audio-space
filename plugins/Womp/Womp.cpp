// Womp.cpp
// Womp atom implementation - combines Kick and Bassline
// AudioSpace (cp.potter@proton.me)

#include "SC_PlugIn.hpp"
#include "../Atom/Atom.hpp"
#include "Womp.hpp"
#include <cmath>

static InterfaceTable* ft;

namespace Atom {

// ============================================================================
// Womp - Combines kick and bassline with synchronized peaks
// ============================================================================

Womp::Womp() {
    mCalcFunc = make_calc_function<Womp, &Womp::next>();
    
    // Initialize atom data
    mData = new AtomData();
    mData->id = 0;
    mData->position = Vec3(0.0f, 0.0f, 0.0f);
    mData->velocity = Vec3(0.0f, 0.0f, 0.0f);
    mData->collisionRadius = 0.8f;
    mData->parent = nullptr;
    mData->active = true;
    mData->age = 0.0f;
    mData->audioFreq = 110.0f;
    mData->audioAmp = 0.2f;
    
    // Initialize kick-specific parameters
    mKickPhase = 0.0f;
    mKickFreq = 60.0f;
    mKickEnv = 0.0f;
    mSampleCounter = 0.0f;
    
    // Calculate decay rate for kick envelope
    float kickDecayTime = 0.15f;  // 150ms decay
    float sr = mWorld->mSampleRate;
    mKickDecayRate = std::exp(-1.0f / (kickDecayTime * sr));
    
    // Initialize bassline-specific parameters
    mBassPhase = 0.0f;
    mModPhase = 0.0f;  // Start at 0 so peak aligns with kick
    mBaseFreq = 110.0f;
    mModFreq = 2.0f;
    mModDepth = 0.1f;
    mDistortionAmount = 0.3f;
    
    // Initialize sidechain compression parameters
    mCompressorEnv = 0.0f;
    float compressorAttackTime = 0.01f;  // 10ms attack
    float compressorReleaseTime = 0.1f;  // 100ms release
    mCompressorAttack = std::exp(-1.0f / (compressorAttackTime * sr));
    mCompressorRelease = std::exp(-1.0f / (compressorReleaseTime * sr));
    
    mSpaceId = 0;
    mIsRegistered = false;
    
    // Register with global registry
    auto& registry = AtomRegistry::getInstance();
    mData->id = registry.registerAtom(mData);
    mIsRegistered = true;
    
    next(1);
}

void Womp::next(int nSamples) {
    float* outbuf = out(0);
    float sr = mWorld->mSampleRate;
    float dt = 1.0f / sr;
    
    // Update parameters from inputs
    // Inputs: x, y, z, vx, vy, vz, radius, kickFreq, bassFreq, amp, bpm, kickDecay, modDepth, distortion
    mData->position = Vec3(in0(0), in0(1), in0(2));
    mData->velocity = Vec3(in0(3), in0(4), in0(5));
    mData->collisionRadius = in0(6) > 0.0f ? in0(6) : 0.8f;
    
    float kickFreq = in0(7) > 0.0f ? in0(7) : 60.0f;
    float bassFreq = in0(8) > 0.0f ? in0(8) : 110.0f;
    mData->audioAmp = in0(9);
    
    // Read BPM from input (input 10)
    float bpm = numInputs() > 10 ? in0(10) : 120.0f;
    if (bpm <= 0.0f) bpm = 120.0f;
    
    // Update global registry
    auto& registry = AtomRegistry::getInstance();
    registry.setBPM(bpm);
    
    // Calculate samples per beat for kick timing
    float samplesPerBeat = (sr * 60.0f) / bpm;
    
    // Modulation parameters
    float kickDecayTime = numInputs() > 11 ? in0(11) : 0.15f;
    mModDepth = numInputs() > 12 ? in0(12) : 0.1f;
    mDistortionAmount = numInputs() > 13 ? in0(13) : 0.3f;
    
    // Sidechain compression parameters
    float compressorAttackTime = numInputs() > 14 ? in0(14) : 0.01f;
    float compressorReleaseTime = numInputs() > 15 ? in0(15) : 0.1f;
    float sidechainAmount = numInputs() > 16 ? in0(16) : 0.8f;  // How much to duck (0-1)
    
    // Update compressor time constants
    mCompressorAttack = std::exp(-1.0f / (compressorAttackTime * sr));
    mCompressorRelease = std::exp(-1.0f / (compressorReleaseTime * sr));
    
    // Update kick decay rate
    mKickDecayRate = std::exp(-1.0f / (kickDecayTime * sr));
    
    // Bassline modulation frequency synchronized to BPM (once per beat)
    mModFreq = bpm / 60.0f;
    mBaseFreq = bassFreq;
    mKickFreq = kickFreq;
    
    // Update age
    for (int i = 0; i < nSamples; ++i) {
        mData->age += dt;
    }
    
    // Generate womp sound (kick + bassline)
    float modPhaseInc = 4.0f * static_cast<float>(M_PI) * mModFreq / sr;
    
    for (int i = 0; i < nSamples; ++i) {
        // Check for kick trigger (every beat)
        bool kickTriggered = false;
        if (mSampleCounter >= samplesPerBeat) {
            mSampleCounter -= samplesPerBeat;
            kickTriggered = true;
            
            // Reset modulation phase to align bassline peak with kick
            // Set modPhase to pi/2 (90 degrees) so sin peaks when kick hits
            mModPhase = static_cast<float>(M_PI) * 0.5f;
        }
        mSampleCounter += 1.0f;
        
        // Trigger kick envelope
        if (kickTriggered) {
            mKickEnv = 1.0f;
            mKickFreq = kickFreq * 3.0f;  // Start at 3x for pitch sweep
        }
        
        // Update kick envelope and frequency sweep
        if (mKickEnv > 0.0f) {
            mKickEnv *= mKickDecayRate;
            if (mKickEnv < 0.0001f) {
                mKickEnv = 0.0f;
            }
        }
        
        float targetKickFreq = kickFreq;
        float freqDecayRate = 0.9995f;
        if (mKickFreq > targetKickFreq) {
            mKickFreq *= freqDecayRate;
            if (mKickFreq < targetKickFreq) {
                mKickFreq = targetKickFreq;
            }
        }
        
        // Generate kick sound
        float kickPhaseInc = 2.0f * static_cast<float>(M_PI) * mKickFreq / sr;
        float kickSample = std::sin(mKickPhase) * mKickEnv * mData->audioAmp * 0.5f;
        
        mKickPhase += kickPhaseInc;
        if (mKickPhase > 2.0f * static_cast<float>(M_PI)) {
            mKickPhase -= 2.0f * static_cast<float>(M_PI);
        }
        
        // Generate bassline with frequency modulation
        // Modulation signal (sine wave) - synchronized to kick
        float modSignal = std::sin(mModPhase);
        
        // Modulate frequency
        float currentFreq = mBaseFreq * (1.0f + mModDepth * modSignal);
        
        // Calculate phase increment for bassline oscillator
        float bassPhaseInc = 2.0f * static_cast<float>(M_PI) * currentFreq / sr;
        
        // Generate bassline sine wave
        float bassSample = std::sin(mBassPhase);
        
        // Apply distortion at peaks - when modulation is at peak (aligned with kick)
        float absModSignal = std::abs(modSignal);
        if (absModSignal > 0.7f) {  // Near the peak of modulation
            float peakFactor = (absModSignal - 0.7f) / 0.3f;
            
            float freqDeviation = std::abs(currentFreq - mBaseFreq) / mBaseFreq;
            float distortion = peakFactor * freqDeviation * mDistortionAmount;
            
            // Add harmonics/distortion
            bassSample += distortion * std::sin(mBassPhase * 2.0f) * 0.3f;
            bassSample += distortion * std::sin(mBassPhase * 3.0f) * 0.15f;
            
            // Soft clipping
            if (bassSample > 1.0f) bassSample = 1.0f - (1.0f - 1.0f / bassSample) * 0.5f;
            if (bassSample < -1.0f) bassSample = -1.0f + (1.0f + 1.0f / bassSample) * 0.5f;
        }
        
        float bassOutput = bassSample * mData->audioAmp * 0.7f;
        
        // Sidechain compression: use kick envelope to compress bassline
        // Use the kick envelope as the control signal (smoother than raw kick sample)
        float kickEnvLevel = mKickEnv;
        
        // Envelope follower for compression control
        if (kickEnvLevel > mCompressorEnv) {
            // Attack phase - fast response to kick
            mCompressorEnv = kickEnvLevel + (mCompressorEnv - kickEnvLevel) * mCompressorAttack;
        } else {
            // Release phase - slow decay
            mCompressorEnv = kickEnvLevel + (mCompressorEnv - kickEnvLevel) * mCompressorRelease;
        }
        
        // Apply sidechain compression to bassline
        // When kick envelope is strong, compress bassline more
        // Create compression curve: more compression when kick envelope is high
        float compressionRatio = 0.1f;  // Strong compression (10:1 ratio)
        float threshold = 0.05f;  // Threshold to start compression
        
        float compressorGain = 1.0f;
        if (mCompressorEnv > threshold) {
            // Calculate compression gain reduction
            float overThreshold = mCompressorEnv - threshold;
            float compressedLevel = threshold + (overThreshold / compressionRatio);
            compressorGain = compressedLevel / (mCompressorEnv + 0.0001f);  // Avoid division by zero
            
            // Apply sidechain amount - blend between no compression and full compression
            compressorGain = 1.0f - (1.0f - compressorGain) * sidechainAmount;
        }
        
        // Apply compression to bassline - duck it when kick plays
        bassOutput *= compressorGain;
        
        // Update phases
        mBassPhase += bassPhaseInc;
        if (mBassPhase > 2.0f * static_cast<float>(M_PI)) {
            mBassPhase -= 2.0f * static_cast<float>(M_PI);
        }
        
        mModPhase += modPhaseInc;
        if (mModPhase > 2.0f * static_cast<float>(M_PI)) {
            mModPhase -= 2.0f * static_cast<float>(M_PI);
        }
        
        // Mix kick and compressed bassline
        outbuf[i] = kickSample + bassOutput;
    }
}

// ============================================================================
// Plugin registration
// ============================================================================

PluginLoad(WompUGens) {
    ft = inTable;
    registerUnit<Womp>(ft, "Womp", false);
}

} // namespace Atom
