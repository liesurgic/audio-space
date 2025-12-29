// Atom.hpp
// Music Universe with Atomic 3D Audio Space
// AudioSpace (cp.potter@proton.me)

#pragma once

#include "SC_PlugIn.hpp"
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <map>

namespace Atom {

// Forward declarations
class Atom;
class PhysicsComponent;
class CollisionHandler;

// 3D Vector structure
struct Vec3 {
    float x, y, z;
    
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    
    float length() const { return std::sqrt(x*x + y*y + z*z); }
    float distance(const Vec3& other) const { return (*this - other).length(); }
    Vec3 normalize() const { float len = length(); return len > 0.0f ? Vec3(x/len, y/len, z/len) : Vec3(); }
};

// Base Atom data structure - minimal core properties
struct AtomData {
    int32 id;                    // Unique identifier
    Vec3 position;               // 3D position in space
    Vec3 velocity;               // 3D velocity for movement
    float collisionRadius;       // Radius for collision detection
    AtomData* parent;            // Parent atom (for hierarchy)
    std::vector<AtomData*> children;  // Child atoms
    bool active;                 // Whether atom is active
    float age;                   // Age of atom in seconds
    
    // Audio generation properties - to be used by derived classes
    float audioFreq;             // Frequency for audio generation
    float audioAmp;              // Audio amplitude
};

// Abstract base PhysicsComponent - can be composed into atoms
class PhysicsComponent {
public:
    virtual ~PhysicsComponent() = default;
    virtual void update(AtomData& atom, float dt) = 0;
};

// Abstract base CollisionHandler - handles collisions between atoms
class CollisionHandler {
public:
    virtual ~CollisionHandler() = default;
    virtual void handleCollision(AtomData& atom1, AtomData& atom2) = 0;
};

// Global atom registry for managing atoms across UGens
class AtomRegistry {
public:
    static AtomRegistry& getInstance() {
        static AtomRegistry instance;
        return instance;
    }
    
    int32 registerAtom(AtomData* atom) {
        int32 id = mNextId++;
        mAtoms[id] = atom;
        return id;
    }
    
    void unregisterAtom(int32 id) {
        mAtoms.erase(id);
    }
    
    AtomData* getAtom(int32 id) {
        auto it = mAtoms.find(id);
        return (it != mAtoms.end()) ? it->second : nullptr;
    }
    
    std::vector<AtomData*> getAllAtoms() {
        std::vector<AtomData*> result;
        for (auto& pair : mAtoms) {
            if (pair.second && pair.second->active) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
    
    void clear() {
        mAtoms.clear();
        mNextId = 1;
    }
    
    // Global BPM management
    void setBPM(float bpm) {
        mBPM = bpm > 0.0f ? bpm : 120.0f;
    }
    
    float getBPM() const {
        return mBPM;
    }
    
    // Calculate samples per beat based on current sample rate
    float getSamplesPerBeat(float sampleRate) const {
        return (sampleRate * 60.0f) / mBPM;
    }
    
    // Calculate samples per bar (assuming 4/4 time)
    float getSamplesPerBar(float sampleRate) const {
        return getSamplesPerBeat(sampleRate) * 4.0f;
    }
    
private:
    std::map<int32, AtomData*> mAtoms;
    int32 mNextId = 1;
    float mBPM = 120.0f;  // Default BPM
};

// AtomSpace UGen - Manages the universe (container space)
class AtomSpace : public SCUnit {
public:
    AtomSpace();
    
private:
    void next(int nSamples);
    
    float mSpaceSize;            // Size of the space (cube with side length)
};

// Base Atom UGen - Minimal base class
// Note: Cannot use virtual functions (SuperCollider constraint)
// Concrete atom types should be separate classes sharing AtomData
class Atom : public SCUnit {
public:
    Atom();
    
protected:
    // Core atom data
    AtomData* mData;
    
    // Components that can be added (non-virtual composition)
    std::vector<std::unique_ptr<PhysicsComponent>> mPhysicsComponents;
    std::unique_ptr<CollisionHandler> mCollisionHandler;
    
    // Basic audio generation state
    float mPhase;
    
    // Helper methods
    void updatePosition(float dt);
    void updatePhysics(float dt);
    
    int32 mSpaceId;
    bool mIsRegistered;
    
private:
    void next(int nSamples);
    
    // Audio generation - concrete implementation (no virtual functions)
    void generateAudio(float* outbuf, int nSamples, float sr);
    void updateBehavior(float dt);
};

} // namespace Atom
