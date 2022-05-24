#include "jackassignmentunknown.h"


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

JackAssignment *JackAssignmentUnknown::clone() const
{
    return new JackAssignmentUnknown(jack, comment, atom ? atom->toString() : "");
}
