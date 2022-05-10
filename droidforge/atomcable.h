#ifndef ATOMCABLE_H
#define ATOMCABLE_H

#include <QString>

#include "atom.h"

class AtomCable : public Atom
{
public:
    QString name;
    AtomCable(QString n) : name(n) {};
    AtomCable *clone() const;
    QString toString() const;
    bool isCable() const { return true; };
};

#endif // ATOMCABLE_H
