#ifndef ATOM_H
#define ATOM_H

class PatchProblem;

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
    virtual bool isCable() const { return false; };
    virtual bool isInvalid() const { return false; };
    virtual bool needG8() const { return false; };
    virtual bool needX7() const { return false; };
    virtual QString problemAsInput(const Patch *patch) const = 0;
    virtual QString problemAsOutput(const Patch *patch) const = 0;
};

#endif // ATOM_H
