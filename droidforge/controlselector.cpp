#include "controlselector.h"


ControlSelector::ControlSelector(QWidget *parent)
    : RegisterSelector(true, 'B', "BLPSR", parent)
{
    addRegisterButton('B', tr("Button"));
    addRegisterButton('L', tr("LED in Button"));
    addRegisterButton('P', tr("Potentiometer"));
    addRegisterButton('S', tr("Swich"));
    addRegisterButton('R', tr("RGB-LED"));
}


bool ControlSelector::handlesAtom(const Atom *atom) const
{
    return atom->isRegister() &&
            ((AtomRegister *)atom)->isControl();

}
