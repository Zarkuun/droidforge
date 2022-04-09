#ifndef ATOM_H
#define ATOM_H

#include <QString>

class Atom
{
public:
    Atom() {};
    virtual ~Atom() {};
    virtual QString toString() = 0;
};

#endif // ATOM_H
