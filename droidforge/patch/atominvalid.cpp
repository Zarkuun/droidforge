#include "atominvalid.h"
#include "patchproblem.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)


AtomInvalid *AtomInvalid::clone() const
{
    return new AtomInvalid(rawtext);
}

QList<PatchProblem *> AtomInvalid::collectProblemsAsInput(const Patch *) const
{
    QList<PatchProblem *>problems;
    problems.append(
                new PatchProblem(0, 0, tr("Invalid (garbled) value")));
    return problems;
}

QList<PatchProblem *> AtomInvalid::collectProblemsAsOutput(const Patch *) const
{
    QList<PatchProblem *>problems;
    problems.append(
                new PatchProblem(0, 0, tr("Invalid value or value that cannot be used as output")));
    return problems;
}
