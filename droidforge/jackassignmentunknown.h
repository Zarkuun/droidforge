#ifndef JACKASSIGNMENTUNKNOWN_H
#define JACKASSIGNMENTUNKNOWN_H

#include "jackassignment.h"

class JackAssignmentUnknown : public JackAssignment
{
    QString valueString;

public:
    JackAssignmentUnknown(QString jack, QString comment="", QString valueString="");
    QString valueToString() const { return valueString; };
    jacktype_t jackType() const { return JACKTYPE_UNKNOWN; };

protected:
    JackAssignment *clone() const;
};

#endif // JACKASSIGNMENTUNKNOWN_H
