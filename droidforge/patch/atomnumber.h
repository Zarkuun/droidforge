#ifndef ATOMNUMBER_H
#define ATOMNUMBER_H

#include "atom.h"

typedef enum {
    ATOM_NUMBER_NUMBER,
    ATOM_NUMBER_VOLTAGE,
    ATOM_NUMBER_PERCENTAGE,
    ATOM_NUMBER_ONOFF
} atom_number_t;


class AtomNumber : public Atom
{
    float number; // 1V is 0.1. For 1/4 is 0.25, not 4
    atom_number_t numberType;
    bool fraction; // true -> output as 1/X fraction (only for Atom B)

public:
    AtomNumber(float n, atom_number_t t, bool fraction)
        : number(n), numberType(t), fraction(fraction) {};
    float getNumber() const { return number; };
    atom_number_t getType() const { return numberType; };
    AtomNumber *clone() const;
    bool isFraction() const { return fraction; };
    QString toString() const;
    QString toDisplay() const;
    bool isNegatable() const;
    QString toNegatedString() const;
    bool isNumber() const { return true; };
    QString problemAsInput(const Patch *patch) const;
    QString problemAsOutput(const Patch *patch) const;

private:
    QString toFractionString(float number) const;

};

#endif // ATOMNUMBER_H
