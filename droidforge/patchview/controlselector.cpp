#include "controlselector.h"


ControlSelector::ControlSelector(jacktype_t jacktype, QWidget *parent)
    : RegisterSelector(
          true,
          jacktype == JACKTYPE_INPUT ? REGISTER_BUTTON : REGISTER_LED,
          jacktype == JACKTYPE_INPUT ? "BLPS" : "LS", parent)
{
    if (jacktype == JACKTYPE_INPUT) {
        addRegisterButton(REGISTER_BUTTON, tr("Button"));
        addRegisterButton(REGISTER_LED, tr("LED in Button"));
        addRegisterButton(REGISTER_POT, tr("Potentiometer"));
        addRegisterButton(REGISTER_SWITCH, tr("Swich"));
    }
    else {
        addRegisterButton(REGISTER_LED, tr("LED in Button"));
        addRegisterButton(REGISTER_SWITCH, tr("Swich"));
    }
}


bool ControlSelector::handlesAtom(const Atom *atom) const
{
    return atom->isRegister() &&
            ((AtomRegister *)atom)->isControl();

}
