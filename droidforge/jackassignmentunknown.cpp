#include "jackassignmentunknown.h"


JackAssignmentUnknown::JackAssignmentUnknown(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , valueString(valueString)
{
}

void JackAssignmentUnknown::replaceAtom(int, Atom *newAtom)
{
    valueString = newAtom->toString();
}

void JackAssignmentUnknown::parseExpression(const QString &expression)
{
    valueString = expression;
}

JackAssignment *JackAssignmentUnknown::clone() const
{
    return new JackAssignmentUnknown(jack, comment, valueString);
}
