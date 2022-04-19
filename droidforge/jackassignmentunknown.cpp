#include "jackassignmentunknown.h"


JackAssignmentUnknown::JackAssignmentUnknown(QString jack, QString valueString)
    : JackAssignment(jack)
    , valueString(valueString)
{
}


JackAssignment *JackAssignmentUnknown::clone() const
{
    return new JackAssignmentUnknown(jack, valueString);
}
