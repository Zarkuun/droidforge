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

QList<PatchProblem *> AtomCable::collectProblemsAsInput(const Patch *patch) const
{
    QList<PatchProblem *> problems;

    int numAsOutput = 0;
    int numAsInput = 0;
    patch->findCableConnections(name, numAsInput, numAsOutput);
    if (numAsOutput == 0)
        problems.append(
                    new PatchProblem(0, 0, tr("There is no output connected to this cable")));
    if (numAsInput == 0)
        problems.append(
                    new PatchProblem(0, 0, tr("There is no input connected to this cable")));
    if (numAsOutput > 1)
        problems.append(
                    new PatchProblem(0, 0, tr("There is more than one output connected to this cable")));
    return problems;
}

QList<PatchProblem *> AtomCable::collectProblemsAsOutput(const Patch *patch) const
{
    return collectProblemsAsInput(patch);
}
