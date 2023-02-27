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
    Atom *getAtom() const;
    QString valueToString() const;
    bool isOutput() const { return true; };
    const Atom *atomAt(int a) const { return a >= 1 ? atom : 0; };
    Atom *atomAt(int a) { return a >= 1 ? atom : 0; };
    void replaceAtom(int column, Atom *newAtom);
    void collectCables(QStringList &cables) const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    void parseExpression(const QString &expression);
    static Atom *parseOutputAtom(const QString &s);
    void removeRegisterReferences(RegisterList &rl);
    QList<PatchProblem *> collectSpecificProblems(const Patch *patch) const;
    bool isUndefined() const { return atom == 0; };

protected:
    jacktype_t jackType() const { return JACKTYPE_OUTPUT; };
    JackAssignment *clone() const;

private:
    // void parseOutputValue(QString valueString);

};

#endif // JACKASSIGNMENTOUTPUT_H
