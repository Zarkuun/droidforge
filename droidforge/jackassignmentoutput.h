#ifndef JACKASSIGNMENTOUTPUT_H
#define JACKASSIGNMENTOUTPUT_H

#include "jackassignment.h"

class JackAssignmentOutput : public JackAssignment
{
    Atom *atom;

public:
    JackAssignmentOutput(QString jack, QString comment, QString valueString);
    JackAssignmentOutput(QString jack, QString comment="");
    ~JackAssignmentOutput();
    Atom *getAtom() const;;
    QString valueToString() const;

protected:
    jacktype_t jackType() const { return JACKTYPE_OUTPUT; };
    JackAssignment *clone() const;

private:
    void parseOutputValue(QString valueString);
};

#endif // JACKASSIGNMENTOUTPUT_H
