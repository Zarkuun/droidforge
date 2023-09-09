#ifndef INPUTOUTPUTSELECTOR_H
#define INPUTOUTPUTSELECTOR_H

#include "registerselector.h"

#include <QComboBox>

class InputOutputSelector : public RegisterSelector
{
    // jacktype_t jacktype;

public:
    explicit InputOutputSelector(jacktype_t, QWidget *parent = nullptr);
    QString title() const;
    bool handlesAtom(const Atom *atom) const;

protected:
    bool isControl() { return false; };
};

#endif // INPUTOUTPUTSELECTOR_H
