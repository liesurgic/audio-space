// PluginAtom.cpp
// SwampMonster (liesurgic@site.com)

#include "SC_PlugIn.hpp"
#include "Atom.hpp"

static InterfaceTable* ft;

namespace Atom {

Atom::Atom() {
    mCalcFunc = make_calc_function<Atom, &Atom::next>();
    next(1);
}

void Atom::next(int nSamples) {

    // Audio rate input
    const float* input = in(0);

    // Control rate parameter: gain.
    const float gain = in0(1);

    // Output buffer
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain;
    }
}

} // namespace Atom

PluginLoad(AtomUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<Atom::Atom>(ft, "Atom", false);
}
