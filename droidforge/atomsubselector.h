#ifndef ATOMSUBSELECTOR_H
#define ATOMSUBSELECTOR_H

#include "atom.h"
#include "patch.h"

#include <QGroupBox>
#include <QObject>

class AtomSubSelector : public QGroupBox
{
public:
    AtomSubSelector(QWidget *parent = nullptr);
    virtual QString title() const = 0;
    virtual void clearAtom() = 0;
    virtual Atom *getAtom() const = 0;
    virtual void getFocus() = 0;
    virtual bool handlesAtom(const Atom *atom) const = 0;
    virtual void setAtom(const Patch *patch, const Atom *atom) = 0;
};

#endif // ATOMSUBSELECTOR_H
