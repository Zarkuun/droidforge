#ifndef JACKASSIGNMENTUNKNOWN_H
#define JACKASSIGNMENTUNKNOWN_H

#include "jackassignment.h"
#include "atominvalid.h"

class JackAssignmentUnknown : public JackAssignment
{
    AtomInvalid *atom;

public:
    JackAssignmentUnknown(QString jack, QString comment="", QString valueString="");
    ~JackAssignmentUnknown();
    QString valueToString() const;
    jacktype_t jackType() const { return JACKTYPE_UNKNOWN; };
    const Atom *atomAt(int) const { return atom; };
    Atom *atomAt(int);
    void replaceAtom(int column, Atom *newAtom);
    void collectCables(QStringList &) const {};
    void parseExpression(const QString &expression);
    void removeRegisterReferences(RegisterList &, int, int) {};
    QList<PatchProblem *> collectProblems(const Patch *patch) const;

protected:
    JackAssignment *clone() const;
};

#endif // JACKASSIGNMENTUNKNOWN_H
