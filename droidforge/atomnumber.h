#ifndef ATOMNUMBER_H
#define ATOMNUMBER_H

#include "atom.h"

typedef enum {
    ATOM_NUMBER_NUMBER,
    ATOM_NUMBER_VOLTAGE,
    ATOM_NUMBER_PERCENTAGE,
    ATOM_NUMBER_ONOFF
} atom_number_t;

// TODO: Darstellung 1/120 soll hier nicht automatisch
// in 0.0166 umgewandelt werden. Jetzt muss man das
// aber auch noch kombinieren mit V. 1/12V

class AtomNumber : public Atom
{
public:
    float number;
    atom_number_t numberType;

    AtomNumber(float n, atom_number_t t)
        : number(n), numberType(t) {};
    AtomNumber *clone() const;
    QString toString() const;
    bool isNegatable() const;
    QString toNegatedString() const;
};

#endif // ATOMNUMBER_H
