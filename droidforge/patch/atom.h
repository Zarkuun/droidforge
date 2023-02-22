#ifndef ATOM_H
#define ATOM_H

class PatchProblem;

#include "rewritecablesdialog.h"

#include <QString>
#include <QList>

class Patch;

class Atom
{
public:
    Atom() {};
    virtual ~Atom() {};
    virtual QString toString() const = 0;
    virtual QString toDisplay() const { return toString(); };
    virtual Atom *clone() const = 0;
    virtual bool isNegatable() const { return false; };
    virtual QString toNegatedString() const { return ""; };
    virtual bool isNumber() const { return false; };
    virtual bool isRegister() const { return false; };
    virtual bool canHaveLabel() const { return false; };
    virtual bool isCable() const { return false; };
    virtual bool isInvalid() const { return false; };
    virtual bool needsG8() const { return false; };
    virtual bool needsX7() const { return false; };
    virtual QString problemAsInput(const Patch *patch) const = 0;
    virtual QString problemAsOutput(const Patch *patch) const = 0;
    virtual void rewriteCableNames(const QString &, const QString &, RewriteCablesDialog::mode_t) {};
    virtual void incrementForExpansion(const Patch *) {};
};

#endif // ATOM_H
