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
    virtual QString toNegatedString() const { return ""; };
    virtual bool isNumber() const { return false; };
    virtual bool isRegister() const { return false; };
    virtual bool isCable() const { return false; };
    virtual bool isInvalid() const { return false; };
    virtual bool needG8() const { return false; };
    virtual bool needX7() const { return false; };
    virtual void swapControllerNumbers(int, int) {};
};

#endif // ATOM_H
