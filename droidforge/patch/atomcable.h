#ifndef ATOMCABLE_H
#define ATOMCABLE_H

#include "atom.h"

#include <QString>

class AtomCable : public Atom
{
    QString name;

public:
    AtomCable(QString n) : name(n) {};
    AtomCable *clone() const override;
    QString toString() const override;
    bool isCable() const  override{ return true; };
    QString getCable() const { return name; };
    void setCable(const QString &n) { name = n; };
    QString problemAsInput(const Patch *patch) const override;
    QString problemAsOutput(const Patch *patch) const override;
    static QString nextCableName(const QString &name);
    void rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode) override;
    void incrementForExpansion(const Patch *patch) override;
};

#endif // ATOMCABLE_H
