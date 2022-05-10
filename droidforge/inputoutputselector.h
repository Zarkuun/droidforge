#ifndef INPUTOUTPUTSELECTOR_H
#define INPUTOUTPUTSELECTOR_H

#include "registerselector.h"

class InputOutputSelector : public RegisterSelector
{
public:
    explicit InputOutputSelector(QWidget *parent = nullptr);

protected:
    bool isControl() { return false; };
};

#endif // INPUTOUTPUTSELECTOR_H
