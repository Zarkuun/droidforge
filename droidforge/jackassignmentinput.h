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
    Atom *atomAt(int column);
    void replaceAtom(int column, Atom *newAtom);
    void removeRegisterReferences(RegisterList &rl, int ih, int);
    QString valueToString() const;
    void collectCables(QStringList &cables) const;
    static Atom *parseInputAtom(const QString &s);

protected:
    jacktype_t jackType() const { return JACKTYPE_INPUT; };
    JackAssignment *clone() const;
    void parseExpression(const QString &expression);

private:
    void parseInputExpression(QString, QString valueString);
    static Atom *parseOnOff(QString s);
    static Atom *parseNumber(QString s);
    bool isInRegisterList(const RegisterList &rl, Atom *atom);
};

#endif // JACKASSIGNMENTINPUT_H
