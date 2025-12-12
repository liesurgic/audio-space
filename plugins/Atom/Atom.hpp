// PluginAtom.hpp
// SwampMonster (liesurgic@site.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace Atom {

class Atom : public SCUnit {
public:
    Atom();

    // Destructor
    // ~Atom();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace Atom
