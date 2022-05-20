#ifndef ATOMINVALID_H
#define ATOMINVALID_H

#include "atom.h"

#include <QString>

class AtomInvalid : public Atom
{
    QString rawtext;

public:
    AtomInvalid(const QString &r) : rawtext(r) {};
    AtomInvalid *clone() const;
    QString toString() const { return rawtext; };
    bool isInvalid() const { return true; };
};

#endif // ATOMINVALID_H
