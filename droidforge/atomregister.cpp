#include "atomregister.h"

QString AtomRegister::toString()
{
    if (controller)
        return registerType + QString::number(controller) + "." + QString::number(number);
    else
        return registerType + QString::number(number);
}
