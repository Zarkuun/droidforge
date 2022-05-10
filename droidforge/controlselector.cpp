#include "controlselector.h"


ControlSelector::ControlSelector(QWidget *parent)
    : RegisterSelector('B', parent)
{
    addRegisterButton('B', tr("Button"));
    addRegisterButton('L', tr("LED in Button"));
    addRegisterButton('P', tr("Potentiometer"));
    addRegisterButton('S', tr("Swich"));
    addRegisterButton('R', tr("RGB-LED"));
}
