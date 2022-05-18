#include "atomregister.h"

#include <QStringList>

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
    return registerType == REGISTER_GATE
           && controller == 0
           && number >= 1
           && number <= 8;
}

bool AtomRegister::needX7() const
{
    return registerType == REGISTER_GATE
           && controller == 0
           && number >= 9
            && number <= 12;
}

void AtomRegister::swapControllerNumbers(int fromindex, int toindex)
{
    if ((int)controller == fromindex)
        controller = toindex;
    else if ((int)controller == toindex)
        controller = fromindex;
}

void AtomRegister::collectRegisterAtoms(QStringList &l) const
{
    l.append(toString());
}
