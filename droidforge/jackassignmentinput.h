#ifndef JACKASSIGNMENTINPUT_H
#define JACKASSIGNMENTINPUT_H

#include "jackassignment.h"

class JackAssignmentInput : public JackAssignment
{
    Atom *atomA; // mult 1
    Atom *atomB; // mult 2
    Atom *atomC; // add

public:
    JackAssignmentInput(QString jack, QString comment, QString valueString);
    ~JackAssignmentInput();
    Atom *getAtom(unsigned n) const;
    QString valueToString() const;

protected:
    jacktype_t jackType() const { return JACKTYPE_INPUT; };
    JackAssignment *clone() const;

private:
    JackAssignmentInput(QString jack, QString comment);
    void parseInputValue(QString jack, QString valueString);
    Atom *parseInputAtom(QString s);
    Atom *parseOnOff(QString s);
    Atom *parseNumber(QString s);
};

#endif // JACKASSIGNMENTINPUT_H
