#include "atomregister.h"
#include "patchproblem.h"
#include "tuning.h"
#include "patch.h"

#include <QStringList>
#include <QDebug>
#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

AtomRegister::AtomRegister()
{
    data.raw = 0;
}

AtomRegister::AtomRegister(QChar t, unsigned c, unsigned n)
{
    data.r.registerType = t.toLatin1();
    data.r.controller = c;
    data.r.number = n;
    data.r.reserved = 0;
}

AtomRegister::AtomRegister(uint32_t raw)
{
    data.raw = raw;
}

AtomRegister::AtomRegister(const AtomRegister &ar)
{
    data.raw = ar.data.raw;
}

AtomRegister AtomRegister::operator=(const AtomRegister &ar)
{
    data.raw = ar.data.raw;
    return *this;
}

AtomRegister *AtomRegister::clone() const
{
    return new AtomRegister(*this);
}

QString AtomRegister::toString() const
{
    if (data.r.controller)
        return data.r.registerType + QString::number(data.r.controller) + "." + QString::number(data.r.number);
    else
        return data.r.registerType + QString::number(data.r.number);
}

QString AtomRegister::toDisplay() const
{
    return QString(registerName(data.r.registerType)) + " " + toString();
}

bool AtomRegister::needG8() const
{
    return data.r.registerType == REGISTER_GATE
           && data.r.controller == 0
           && data.r.number >= 1
           && data.r.number <= 8;
}

bool AtomRegister::needX7() const
{
    return data.r.registerType == REGISTER_GATE
           && data.r.controller == 0
           && data.r.number >= 9
            && data.r.number <= 12;
}

void AtomRegister::swapControllerNumbers(int fromController, int toController)
{
    if ((int)data.r.controller == fromController)
        data.r.controller = toController;
    else if ((int)data.r.controller == toController)
        data.r.controller = fromController;
}

void AtomRegister::shiftControllerNumbers(int controller, int by)
{
    if ((int)data.r.controller > controller)
        data.r.controller += by;
}

QString AtomRegister::problemAsInput(const Patch *patch) const
{
    return generalProblem(patch);
}

QString AtomRegister::problemAsOutput(const Patch *patch) const
{
    switch (data.r.registerType) {
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
    if (data.r.number <= 0)
        return tr("The number of the register may not be less than 1");
    else if (data.r.controller > patch->numControllers())
        return tr("Invalid controller number %1. You have just %2 controllers")
                .arg(data.r.controller).arg(patch->numControllers());
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
    return a.data.raw == b.data.raw;
}

bool operator<(const AtomRegister &a, const AtomRegister &b)
{
    return a.data.r.controller < b.data.r.controller
            || a.data.r.registerType < b.data.r.registerType
            || a.data.r.number < b.data.r.number;
}
