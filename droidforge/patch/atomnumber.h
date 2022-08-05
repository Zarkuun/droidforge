#ifndef ATOMNUMBER_H
#define ATOMNUMBER_H

#include "atom.h"

typedef enum {
    ATOM_NUMBER_NUMBER,
    ATOM_NUMBER_VOLTAGE,
    ATOM_NUMBER_PERCENTAGE,
    ATOM_NUMBER_ONOFF,
    ATOM_NUMBER_FRACTION,
} atom_number_t;


class AtomNumber : public Atom
{
    float number; // 1V is 0.1. For 1/4 is 0.25, not 4
    atom_number_t numberType;

public:
    AtomNumber(float n, atom_number_t t)
        : number(n), numberType(t) {};
    float getNumber() const { return number; };
    atom_number_t getType() const { return numberType; };
    bool isFraction() const { return numberType == ATOM_NUMBER_FRACTION; };
    AtomNumber *clone() const;
    QString toString() const;
    QString toDisplay() const;
    bool isNegatable() const;
    QString toNegatedString() const;
    bool isNumber() const { return true; };
    QString problemAsInput(const Patch *patch) const;
    QString problemAsOutput(const Patch *patch) const;
    static QString niceNumber(float n);

private:
    QString toFractionString(float number) const;

};

#endif // ATOMNUMBER_H
