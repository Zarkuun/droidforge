#include "atomregister.h"

AtomRegister *AtomRegister::clone() const
{
    return new AtomRegister(registerType, controller, number);
}


QString AtomRegister::toString()
{
    if (controller)
        return registerType + QString::number(controller) + "." + QString::number(number);
    else
        return registerType + QString::number(number);
}
