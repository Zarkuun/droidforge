#include "atominvalid.h"
#include "globals.h"
#include "patchproblem.h"


AtomInvalid *AtomInvalid::clone() const
{
    return new AtomInvalid(rawtext);
}

QString AtomInvalid::problemAsInput(const Patch *) const
{
    return TR("Invalid (garbled) value");
}

QString AtomInvalid::problemAsOutput(const Patch *) const
{
    return TR("Invalid value or value that cannot be used as output");
}
