#ifndef ATOM_H
#define ATOM_H

#include <QString>

class Atom
{
public:
    Atom() {};
    virtual ~Atom() {};
    virtual QString toString() const = 0;
    virtual Atom *clone() const = 0;
    virtual bool isNegatable() const { return false; };
    virtual QString toNegatedString() const { return ""; }
};

#endif // ATOM_H
