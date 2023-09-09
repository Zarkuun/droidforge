#ifndef CONTROLSELECTOR_H
#define CONTROLSELECTOR_H

#include "registerselector.h"

class ControlSelector : public RegisterSelector
{
public:
    explicit ControlSelector(jacktype_t, QWidget *parent = nullptr);
    QString title() const { return tr("Controller"); };
    bool handlesAtom(const Atom *atom) const;

protected:
    bool isControl() { return true; };
};

#endif // CONTROLSELECTOR_H
