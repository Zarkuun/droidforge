#include "atominvalid.h"
#include "patchproblem.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)


AtomInvalid *AtomInvalid::clone() const
{
    return new AtomInvalid(rawtext);
}

QString AtomInvalid::problemAsInput(const Patch *patch) const
{
    return tr("Invalid (garbled) value");
}

QString AtomInvalid::problemAsOutput(const Patch *patch) const
{
    return tr("Invalid value or value that cannot be used as output");
}
