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
    JackAssignmentInput(QString jack, QString comment="");
    ~JackAssignmentInput();
    Atom *getAtom(unsigned n) const;
    const Atom *atomAt(int column) const;
    void replaceAtom(int column, Atom *newAtom);
    QString valueToString() const;
    void collectCables(QStringList &cables) const;

protected:
    jacktype_t jackType() const { return JACKTYPE_INPUT; };
    JackAssignment *clone() const;
    void parseExpression(const QString &expression);

private:
    void parseInputExpression(QString jack, QString valueString);
    Atom *parseInputAtom(QString s);
    Atom *parseOnOff(QString s);
    Atom *parseNumber(QString s);
};

#endif // JACKASSIGNMENTINPUT_H
