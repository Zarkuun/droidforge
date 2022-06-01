#ifndef PATCHOPERATOR_H
#define PATCHOPERATOR_H

#include "patch.h"
#include "patcheditengine.h"

/* Helper parent class for all objects that interactively operate
 * on the patch. Contains a pointer to the current
 * patch and all sorts of conveniant access functions
 * to that patch as well as generic operations. */

class PatchOperator
{
protected:
    PatchEditEngine *patch; // borrowed
    PatchSection *section();
    const PatchSection *section() const;

public:
    PatchOperator(PatchEditEngine *patch);
    bool interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch=0);
};

#endif // PATCHOPERATOR_H
