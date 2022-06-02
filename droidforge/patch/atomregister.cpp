#include "atomregister.h"
#include "patchproblem.h"
#include "tuning.h"
#include "patch.h"

#include <QStringList>
#include <QDebug>
#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

AtomRegister::AtomRegister()
    : registerType(0)
    , cont(0)
    , num(0)
{
}

AtomRegister::AtomRegister(char t, unsigned c, unsigned n)
    : registerType(t)
    , cont(c)
    , num(n)
{
}

AtomRegister::AtomRegister(QChar t, unsigned c, unsigned n)
    : registerType(t.toLatin1())
    , cont(c)
    , num(n)
{
}


AtomRegister::AtomRegister(const AtomRegister &ar)
{
    registerType = ar.registerType;
    cont = ar.cont;
    num = ar.num;
}

AtomRegister AtomRegister::operator=(const AtomRegister &ar)
{
    registerType = ar.registerType;
    cont = ar.cont;
    num = ar.num;
    return *this;
}

AtomRegister *AtomRegister::clone() const
{
    return new AtomRegister(*this);
}

QString AtomRegister::toString() const
{
    if (cont)
        return registerType + QString::number(cont) + "." + QString::number(num);
    else
        return registerType + QString::number(num);
}

QString AtomRegister::toDisplay() const
{
    return QString(registerName(registerType)) + " " + toString();
}

bool AtomRegister::needG8() const
{
    return registerType == REGISTER_GATE
           && cont == 0
           && num >= 1
           && num <= 8;
}

bool AtomRegister::needX7() const
{
    return registerType == REGISTER_GATE
           && cont == 0
           && num >= 9
            && num <= 12;
}

void AtomRegister::swapControllerNumbers(int fromController, int toController)
{
    if ((int)cont == fromController)
        cont = toController;
    else if ((int)cont == toController)
        cont = fromController;
}

void AtomRegister::shiftControllerNumbers(int controller, int by)
{
    if ((int)controller > controller)
        controller += by;
}

QString AtomRegister::problemAsInput(const Patch *patch) const
{
    return generalProblem(patch);
}

QString AtomRegister::problemAsOutput(const Patch *patch) const
{
    switch (registerType) {
        case REGISTER_INPUT:
                return tr("You cannot use an input of the master as output");
        case REGISTER_POT:
                return tr("You cannot use a potentiometer as output");
        case REGISTER_BUTTON:
                return tr("You cannot use a button as output");
        case REGISTER_SWITCH:
                return tr("You cannot use a switch as output");
    }
    return generalProblem(patch);
}

QString AtomRegister::generalProblem(const Patch *patch) const
{
    if (num <= 0)
        return tr("The number of the register may not be less than 1");
    else if (cont > patch->numControllers())
        return tr("Invalid controller number %1. You have just %2 controllers")
                .arg(cont).arg(patch->numControllers());
    else if (!patch->registerAvailable(*this))
        return tr("This jack / control is not available");

    return "";
}

QDebug &operator<<(QDebug &out, const AtomRegister &ar) {
    out << ar.toString();
    return out;
}

bool operator==(const AtomRegister &a, const AtomRegister &b)
{
    return a.registerType == b.registerType
            && a.cont == b.cont
            && a.num == b.num;
}

bool operator!=(const AtomRegister &a, const AtomRegister &b)
{
    return !(a == b);
}

bool operator<(const AtomRegister &a, const AtomRegister &b)
{
    return a.cont < b.cont
            || a.registerType < b.registerType
            || a.num < b.num;
}
