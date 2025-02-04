#ifndef ATOMTEXT_H
#define ATOMTEXT_H

#include "atom.h"

class AtomText : public Atom
{
    QString text;

public:
    AtomText(QString t) : text(t) {}
    AtomText *clone() const;
    QString toString() const;
    QString problemAsInput(const Patch *) const { return ""; };
    QString problemAsOutput(const Patch *patch) const;

};

#endif // ATOMTEXT_H
