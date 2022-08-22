#ifndef JACKASSIGNMENTINPUT_H
#define JACKASSIGNMENTINPUT_H

#include "jackassignment.h"

class JackAssignmentInput : public JackAssignment
{
    Atom *atoms[3];

public:
    JackAssignmentInput(QString jack, QString comment, QString valueString);
    JackAssignmentInput(QString jack, QString comment="");
    ~JackAssignmentInput();
    bool isInput() const { return true; };
    Atom *getAtom(int index) { return atoms[index]; };
    virtual int numColumns() const { return 3; };
    const Atom *atomAt(int column) const;
    Atom *atomAt(int column);
    void replaceAtom(int column, Atom *newAtom);
    void removeRegisterReferences(RegisterList &rl);
    QString valueToString() const;
    void collectCables(QStringList &cables) const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    static Atom *parseInputAtom(const QString &s);
    QList<PatchProblem *> collectProblems(const Patch *patch) const;
    bool isUndefined() const;

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
