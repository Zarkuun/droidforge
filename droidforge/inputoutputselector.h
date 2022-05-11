#ifndef INPUTOUTPUTSELECTOR_H
#define INPUTOUTPUTSELECTOR_H

#include "registerselector.h"

class InputOutputSelector : public RegisterSelector
{
public:
    explicit InputOutputSelector(QWidget *parent = nullptr);
    QString title() const { return tr("Input / output"); };
    bool handlesAtom(const Atom *atom) const;

protected:
    bool isControl() { return false; };
};

#endif // INPUTOUTPUTSELECTOR_H
