#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patcheditengine.h"

/* A patch view is a graphical element that shows a part or a
  certain aspect of a patch and does user interaction. It
  does *not* operate on the patch. That is done by
  PatchOperator. */

class PatchView
{
protected:
    PatchEditEngine *patch; // borrowed
    PatchSection *section();
    const PatchSection *section() const;

public:
    PatchView(PatchEditEngine *patch);
    // TODO: Das ist eine Kopie! Rausewrfen!!!
    // bool interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch=0);
};

#endif // PATCHVIEW_H
