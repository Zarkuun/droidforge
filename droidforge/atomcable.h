#ifndef ATOMCABLE_H
#define ATOMCABLE_H

#include <QString>

#include "atom.h"

class AtomCable : public Atom
{
    QString name;

public:
    AtomCable(QString n) : name(n) {};
    AtomCable *clone() const;
    QString toString() const;
    bool isCable() const { return true; };
    QString getCable() const { return name; };
};

#endif // ATOMCABLE_H
