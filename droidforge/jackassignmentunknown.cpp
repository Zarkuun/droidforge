#include "jackassignmentunknown.h"


JackAssignmentUnknown::JackAssignmentUnknown(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , valueString(valueString)
{
}



JackAssignment *JackAssignmentUnknown::clone() const
{
    return new JackAssignmentUnknown(jack, comment, valueString);
}
