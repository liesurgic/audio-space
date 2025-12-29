// Atom.cpp
// Music Universe with Atomic 3D Audio Space
// SwampMonster (liesurgic@site.com)

#include "SC_PlugIn.hpp"
#include "Atom.hpp"
#include <cmath>

static InterfaceTable* ft;

namespace Atom {

// ============================================================================
// AtomSpace UGen - Simple container space
// ============================================================================

AtomSpace::AtomSpace() {
    mCalcFunc = make_calc_function<AtomSpace, &AtomSpace::next>();
    mSpaceSize = in0(0) > 0.0f ? in0(0) : 10.0f;
    next(1);
}

void AtomSpace::next(int nSamples) {
    float* outbuf = out(0);
    mSpaceSize = in0(0) > 0.0f ? in0(0) : 10.0f;
    
    // AtomSpace is just a container - no physics simulation here
    // Output number of active atoms
    auto& registry = AtomRegistry::getInstance();
    auto atoms = registry.getAllAtoms();
    
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = static_cast<float>(atoms.size());
    }
}

// ============================================================================
// Base Atom UGen - Minimal implementation
// ============================================================================

Atom::Atom() {
    mCalcFunc = make_calc_function<Atom, &Atom::next>();
    
    // Initialize minimal atom data
    mData = new AtomData();
    mData->id = 0;
    mData->position = Vec3(0.0f, 0.0f, 0.0f);
    mData->velocity = Vec3(0.0f, 0.0f, 0.0f);
    mData->collisionRadius = 0.5f;
    mData->parent = nullptr;
    mData->active = true;
    mData->age = 0.0f;
    mData->audioFreq = 440.0f;
    mData->audioAmp = 0.1f;
    
    mPhase = 0.0f;
    mSpaceId = 0;
    mIsRegistered = false;
    
    // Register with global registry
    auto& registry = AtomRegistry::getInstance();
    mData->id = registry.registerAtom(mData);
    mIsRegistered = true;
    
    next(1);
}

void Atom::updatePosition(float dt) {
    // Basic position update from velocity
    // Derived classes can override or extend this
    mData->position = mData->position + mData->velocity * dt;
}

void Atom::updatePhysics(float dt) {
    // Update all physics components
    for (auto& component : mPhysicsComponents) {
        if (component) {
            component->update(*mData, dt);
        }
    }
}

void Atom::next(int nSamples) {
    float* outbuf = out(0);
    float sr = mWorld->mSampleRate;
    float dt = 1.0f / sr;
    
    // Update age
    for (int i = 0; i < nSamples; ++i) {
        mData->age += dt;
    }
    
    // Update behavior
    updateBehavior(dt * nSamples);
    
    // Update physics components if any
    updatePhysics(dt * nSamples);
    
    // Generate audio
    generateAudio(outbuf, nSamples, sr);
}

void Atom::updateBehavior(float dt) {
    // Default: do nothing
    // Concrete atom types implement their own behavior
}

void Atom::generateAudio(float* outbuf, int nSamples, float sr) {
    // Default: generate simple sine wave
    if (!mData || !mData->active) {
        for (int i = 0; i < nSamples; ++i) {
            outbuf[i] = 0.0f;
        }
        return;
    }
    
    float phaseInc = 2.0f * static_cast<float>(M_PI) * mData->audioFreq / sr;
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = std::sin(mPhase) * mData->audioAmp;
        mPhase += phaseInc;
        if (mPhase > 2.0f * static_cast<float>(M_PI)) {
            mPhase -= 2.0f * static_cast<float>(M_PI);
        }
    }
}

// ============================================================================
// Plugin registration
// ============================================================================

PluginLoad(AtomUGens) {
    ft = inTable;
    registerUnit<AtomSpace>(ft, "AtomSpace", false);
    // Note: Atom is now a minimal base class - concrete atom types should be derived classes
    // For now, we don't register the base Atom as it's meant to be extended
    // registerUnit<Atom>(ft, "Atom", false);
}

} // namespace Atom
