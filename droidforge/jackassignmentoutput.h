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
    const Atom *atomAt(int) const { return atom; };
    Atom *atomAt(int) { return atom; };
    void replaceAtom(int column, Atom *newAtom);
    void collectCables(QStringList &cables) const;
    void parseExpression(const QString &expression);
    static Atom *parseOutputAtom(const QString &s);

protected:
    jacktype_t jackType() const { return JACKTYPE_OUTPUT; };
    JackAssignment *clone() const;

private:
    // void parseOutputValue(QString valueString);
};

#endif // JACKASSIGNMENTOUTPUT_H
