#ifndef ATOMCABLE_H
#define ATOMCABLE_H

#include "atom.h"

#include <QString>

class AtomCable : public Atom
{
    QString name;

public:
    AtomCable(QString n) : name(n) {};
    AtomCable *clone() const;
    QString toString() const;
    bool isCable() const { return true; };
    QString getCable() const { return name; };
    void setCable(const QString &n) { name = n; };
    QList<PatchProblem *> collectProblemsAsInput(const Patch *patch) const;
    QList<PatchProblem *> collectProblemsAsOutput(const Patch *patch) const;
};

#endif // ATOMCABLE_H
