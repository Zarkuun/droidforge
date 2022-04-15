#include "atomcable.h"

AtomCable *AtomCable::clone() const
{
    return new AtomCable(name);
}

QString AtomCable::toString()
{
    return QString("_") + name;
}
