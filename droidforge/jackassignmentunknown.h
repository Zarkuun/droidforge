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
    const Atom *atomAt(int) const { return 0; };
    void replaceAtom(int column, Atom *newAtom);

protected:
    JackAssignment *clone() const;
};

#endif // JACKASSIGNMENTUNKNOWN_H
