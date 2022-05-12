#include "controlselector.h"


ControlSelector::ControlSelector(jacktype_t jacktype, QWidget *parent)
    : RegisterSelector(
          true,
          jacktype == JACKTYPE_INPUT ? 'B' : 'L',
          jacktype == JACKTYPE_INPUT ? "BLPS" : "LS", parent)
{
    if (jacktype == JACKTYPE_INPUT) {
        addRegisterButton('B', tr("Button"));
        addRegisterButton('L', tr("LED in Button"));
        addRegisterButton('P', tr("Potentiometer"));
        addRegisterButton('S', tr("Swich"));
    }
    else {
        addRegisterButton('L', tr("LED in Button"));
        addRegisterButton('S', tr("Swich"));
    }
}


bool ControlSelector::handlesAtom(const Atom *atom) const
{
    return atom->isRegister() &&
            ((AtomRegister *)atom)->isControl();

}
