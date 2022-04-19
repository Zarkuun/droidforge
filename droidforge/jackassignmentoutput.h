#ifndef JACKASSIGNMENTOUTPUT_H
#define JACKASSIGNMENTOUTPUT_H

#include "jackassignment.h"

class JackAssignmentOutput : public JackAssignment
{
    Atom *atom;

public:
    JackAssignmentOutput(QString jack, QString comment, QString valueString);
    ~JackAssignmentOutput();
    Atom *getAtom() const;;
    QString valueToString() const;

protected:
    jacktype_t jackType() const { return JACKTYPE_OUTPUT; };
    JackAssignment *clone() const;

private:
    JackAssignmentOutput(QString jack, QString comment);
    void parseOutputValue(QString valueString);
};

#endif // JACKASSIGNMENTOUTPUT_H
