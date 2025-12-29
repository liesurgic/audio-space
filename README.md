# Atom

Author: SwampMonster

Atom is a SuperCollider plugin implementing a **Music Universe with Atomic 3D Audio Space** - a system where sound objects (Atoms) exist, move, and interact within a 3D audio space. Each Atom is an autonomous entity that generates audio, moves through space, and responds to collisions with other Atoms.

### Requirements

- CMake >= 3.5
- SuperCollider source code

### Building

Clone the project:

    git clone https://github.com/liesurgic/atom
    cd atom
    mkdir build
    cd build

Then, use CMake to configure and build it:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cmake --build . --config Release --target install

You may want to manually specify the install location in the first step to point it at your
SuperCollider extensions directory: add the option `-DCMAKE_INSTALL_PREFIX=/path/to/extensions`.

It's expected that the SuperCollider repo is cloned at `../supercollider` relative to this repo. If
it's not: add the option `-DSC_PATH=/path/to/sc/source`.

### Developing

Use the command in `regenerate` to update CMakeLists.txt when you add or remove files from the
project. You don't need to run it if you only change the contents of existing files. You may need to
edit the command if you add, remove, or rename plugins, to match the new plugin paths. Run the
script with `--help` to see all available options.

### Usage

After building and installing the plugin, recompile the SuperCollider class library (Language → Recompile Class Library) or restart SuperCollider.

#### Basic Example

Create a space with atoms that move and interact:

```supercollider
// Create a space and two atoms that will collide
{
    var space = AtomSpace.ar(10.0, 0.0, 100);  // space size, gravity, max atoms
    var atom1 = Atom.ar(0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.5, 440.0, 0.1, 2);
    var atom2 = Atom.ar(5.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.5, 880.0, 0.1, 2);
    [atom1, atom2].sum;
}.play;
```

#### Generative Ambient Music

```supercollider
{
    var space = AtomSpace.ar(20.0, 0.1, 100);
    var baseFreq = 110.0;
    var atoms = Array.fill(5, { |i|
        var angle = (i / 5) * 2pi;
        Atom.ar(
            cos(angle) * 2.0, sin(angle) * 2.0, 0.0,
            sin(angle) * 0.2, cos(angle) * 0.2, 0.0,
            0.6, baseFreq * (i + 1), 0.08, 4  // Resonance interaction
        );
    });
    atoms.sum;
}.play;
```

See the Concept section below for more details about the system architecture.


## Concept: Music Universe with Atomic 3D Audio Space

### Vision
This project is a music generation system that creates a **living, evolving music universe** where sound objects (Atoms) exist, move, and interact within a 3D audio space. Each Atom is an autonomous entity that can generate audio, contain other Atoms, and respond to collisions and interactions with other Atoms.

### Core Principles

#### 1. **Atom Objects - The Fundamental Building Blocks**
- **Base Atom Class**: Every sound-generating entity inherits from a base `Atom` class in C++.
- **3D Positioning**: Each Atom has spatial coordinates (`x`, `y`, `z`) within a 3D audio space.
- **Audio Generation**: Atoms can generate and emit audio signals (synthesized sounds, samples, effects, etc.).
- **Spatial Audio**: Audio output is positioned in 3D space using audio spatialization techniques appropriate for C++ platforms.

#### 2. **Hierarchical Composition - Atoms Within Atoms**
- **Parent-Child Relationships**: Atoms can contain other Atoms as children.
- **Local Coordinate Systems**: Child Atoms exist within their parent's local 3D space.
- **Nested Hierarchies**: This creates potentially infinite nesting (atoms within atoms within atoms...).
- **Relative Positioning**: Child positions are relative to their parent's position and orientation.
- **Inheritance of Space**: Children inherit their parent's spatial audio context and transformations.

#### 3. **3D Audio Space Population**
- **Spatial Distribution**: Atoms populate 3D space with audio sources at specific coordinates.
- **Dynamic Movement**: Atoms can move through space over time, creating evolving soundscapes.
- **Distance-Based Effects**: Audio properties (volume, reverb, filtering) can be affected by spatial relationships.
- **Boundary Systems**: Spaces can have boundaries that affect how Atoms behave at edges.

#### 4. **Collision Detection & Interaction**
- **Proximity Detection**: System detects when Atoms come within a certain distance of each other.
- **Collision Events**: When Atoms collide or come into proximity, interaction events are triggered.
- **Dual Interaction Model**: 
  - **Parent-Defined Behavior**: The parent Atom (or the containing Space) can define how two colliding Atoms interact.
  - **Child-Defined Response**: Each Atom can also define its own response to collision events.
- **Interaction Types**: Examples include:
  - **Fusion**: Two Atoms merge to create a new sound.
  - **Repulsion**: Atoms bounce away from each other.
  - **Absorption**: One Atom absorbs another, inheriting its properties.
  - **Resonance**: Atoms create harmonic relationships when near each other.
  - **Modulation**: One Atom modulates another's audio properties.
  - **Triggering**: Collision triggers a sound or sequence in one or both Atoms.

#### 5. **Emergent Musical Behavior**
- **Self-Organizing Systems**: Through collisions and interactions, Atoms naturally form patterns and structures.
- **Evolutionary Composition**: Music evolves from the interactions rather than being pre-composed.
- **Real-Time Generation**: The universe runs in real time, creating live, ever-changing music.
- **User Influence**: Users can create Atoms, modify their properties, and influence the system, but the music is largely emergent.

### Technical Implementation

#### Atom Structure

```cpp
#include <vector>
#include <memory>
#include <array>

class AudioNode; // Abstract audio node base class (platform-dependent implementation)
class InteractionRule; // Abstract interaction rule class

class Atom {
public:
    struct Vec3 {
        float x, y, z;
    };

    Atom();
    virtual ~Atom();

    Vec3 position;
    float collisionRadius;

    std::vector<std::unique_ptr<Atom>> children;
    std::shared_ptr<AudioNode> audioSource;
    std::vector<std::shared_ptr<InteractionRule>> interactionRules;

    // Methods for collision detection, audio generation, child management
    bool checkCollision(const Atom& other) const;
    virtual void onCollision(Atom& other);
    void addChild(std::unique_ptr<Atom> child);

    // ... Other relevant methods
};
```

#### Interaction System
- **Event-Driven**: Collision events trigger interaction handlers (e.g., `onCollision`).
- **Rule-Based**: Each Atom can have custom interaction rules (possibly deriving from `InteractionRule`).
- **Bidirectional**: Both colliding Atoms can respond to the event.
- **Composable**: Interaction behaviors can be combined and extended via C++ inheritance and rule containers.

### Use Cases & Examples

1. **Generative Ambient Music**: Atoms drift through space, creating ambient textures through collisions.
2. **Rhythmic Patterns**: Kick, snare, and hi-hat Atoms orbit each other, triggering on collision.
3. **Melodic Sequences**: Note Atoms follow paths, creating melodies when they intersect.
4. **Sound Design**: Complex soundscapes emerge from simple Atom interactions.
5. **Live Performance**: Performers can inject Atoms into the system and watch them evolve.

### 3D Visualization

The universe includes a real-time 3D visualization (e.g., using OpenGL, Vulkan, or a C++ wrapper for Three.js):

- **Real-Time Rendering**: Atoms are visualized as 3D objects that move in real time.
- **Visual Differentiation**: Different Atom types have unique shapes and colors:
  - **Kick** - Red spheres
  - **Snare** - Cyan cubes
  - **HiHat** - Yellow cones
  - **Orbiter** - Teal octahedrons
- **Collision Visualization**: Wireframe spheres show collision radii.
- **Parent-Child Connections**: Lines connect parent Atoms to their children.
- **Interactive Camera**: 
  - Drag to rotate the view
  - Scroll to zoom in/out
  - Orbit controls for smooth navigation
- **Grid & Axes**: Visual reference for 3D space orientation

The visualization updates automatically as Atoms move and interact, providing a visual representation of the music universe.

### Future Possibilities
- **Physics Simulation**: Gravity, momentum, and forces affecting Atom movement.
- **AI Behavior**: Atoms with learning capabilities that adapt their behavior.
- **Networked Atoms**: Multiple users contributing Atoms to a shared universe.
- **Recording & Playback**: Capture and replay interesting musical moments.
- **Visual Effects**: Particle effects on collisions, trails, and more.



examples high level implementation
universe: extended atom that contains full 3d space which atoms can get injected into