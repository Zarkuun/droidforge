#ifndef ATOMCABLE_H
#define ATOMCABLE_H

#include <QString>

#include "atom.h"

class AtomCable : public Atom
{
public:
    QString name;
    AtomCable(QString n) : name(n) {};
    QString toString();
};

#endif // ATOMCABLE_H
