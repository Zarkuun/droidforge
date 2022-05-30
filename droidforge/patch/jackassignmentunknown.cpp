#include "jackassignmentunknown.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

JackAssignmentUnknown::JackAssignmentUnknown(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , atom(new AtomInvalid(valueString))
{
}

JackAssignmentUnknown::~JackAssignmentUnknown()
{
    if (atom)
        delete atom;
}

QString JackAssignmentUnknown::valueToString() const
{
    if (atom)
        return atom->toString();
    else
        return "";
}

Atom *JackAssignmentUnknown::atomAt(int)
{
    return atom;
}

void JackAssignmentUnknown::replaceAtom(int, Atom *newAtom)
{
    atom = new AtomInvalid(newAtom->toString());
}

void JackAssignmentUnknown::parseExpression(const QString &expression)
{
    if (atom)
        delete atom;
    if (expression.isEmpty())
        atom = 0;
    else
        atom = new AtomInvalid(expression);
}

QList<PatchProblem *> JackAssignmentUnknown::collectProblems(const Patch *) const
{
    QList<PatchProblem *>problems;
    problems.append(
                new PatchProblem(-1, 1, tr("Unknown parameter '%1' in this circuit'").arg(jackName())));
    for (auto problem: problems)
        problem->setColumn(1);
    return problems;
}

JackAssignment *JackAssignmentUnknown::clone() const
{
    return new JackAssignmentUnknown(jack, comment, atom ? atom->toString() : "");
}
