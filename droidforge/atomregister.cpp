#include "atomregister.h"



AtomRegister *AtomRegister::clone() const
{
    return new AtomRegister(registerType, controller, number);
}


QString AtomRegister::toString() const
{
    if (controller)
        return registerType + QString::number(controller) + "." + QString::number(number);
    else
        return registerType + QString::number(number);
}

bool AtomRegister::needG8() const
{
    return registerType == 'G'
           && controller == 0
           && number >= 1
           && number <= 8;
}
