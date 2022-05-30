#include "atomcable.h"
#include "patchproblem.h"
#include "patch.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

AtomCable *AtomCable::clone() const
{
    return new AtomCable(name);
}

QString AtomCable::toString() const
{
    return QString("_") + name;
}

QString AtomCable::problemAsInput(const Patch *patch) const
{
    int numAsOutput = 0;
    int numAsInput = 0;
    patch->findCableConnections(name, numAsInput, numAsOutput);
    if (numAsOutput == 0)
        return tr("There is no output connected to this cable");
    else if (numAsInput == 0)
        return tr("There is no input connected to this cable");
    else if (numAsOutput > 1)
        return tr("There is more than one output connected to this cable");
    else
        return "";
}

QString AtomCable::problemAsOutput(const Patch *patch) const
{
    return problemAsInput(patch);
}
