#ifndef ATOMTEXT_H
#define ATOMTEXT_H

#include "atom.h"

class AtomText : public Atom
{
    QString text; // Must not contain ", only chars 32 .. 126

public:
    AtomText(QString t) : text(t) {};
    AtomText *clone() const;
    QString toString() const;
    bool isText() const { return true; };
    QString getText() const { return text; };
    QString problemAsInput(const Patch *patch) const;
    QString problemAsOutput(const Patch *patch) const;
    static QString cleanText(QString text);
};

#endif // ATOMTEXT_H
