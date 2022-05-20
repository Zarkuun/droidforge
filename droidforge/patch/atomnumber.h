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
    float number; // 1V is 0.1
    atom_number_t numberType;

public:

    AtomNumber(float n, atom_number_t t)
        : number(n), numberType(t) {};
    float getNumber() const { return number; };
    atom_number_t getType() const { return numberType; };
    AtomNumber *clone() const;
    QString toString() const;
    bool isNegatable() const;
    QString toNegatedString() const;
    bool isNumber() const { return true; };
};

#endif // ATOMNUMBER_H
