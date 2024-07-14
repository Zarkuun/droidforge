#include "jackassignmentunknown.h"
#include "globals.h"

#include <QCoreApplication>
#include <QSettings>

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
    QString text = "";
    if (newAtom)
        text = newAtom->toString();
    atom = new AtomInvalid(text);
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

QList<PatchProblem *> JackAssignmentUnknown::collectSpecificProblems(const Patch *) const
{
    QList<PatchProblem *>problems;
    QSettings settings;
    if (!settings.value("validation/ignore_unknown_jacks", false).toBool())
        problems.append(new PatchProblem(-1, 0, TR("Unknown parameter '%1' in this circuit'").arg(jackName())));
    return problems;
}

JackAssignment *JackAssignmentUnknown::clone() const
{
    JackAssignment *ja = new JackAssignmentUnknown(jack, comment, atom ? atom->toString() : "");
    ja->setDisabled(isDisabled());
    return ja;
}
