#include "atom.h"
#include "atomnumber.h"

bool Atom::sameAs(const Atom *other) const
{
    if (isNumber() && other->isNumber()) {
        return ((const AtomNumber *)this)->getNumber() ==
               ((const AtomNumber *)other)->getNumber();
    }
    else
        return toString() == other->toString();
}
