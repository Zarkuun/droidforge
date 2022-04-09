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
public:
    float number;
    atom_number_t numberType;

    AtomNumber(float n, atom_number_t t)
        : number(n), numberType(t) {};
    QString toString();
};

#endif // ATOMNUMBER_H
