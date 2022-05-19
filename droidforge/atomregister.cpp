#include "atomregister.h"

#include <QStringList>
#include <QDebug>

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

void AtomRegister::swapControllerNumbers(int fromNumber, int toNumber)
{
    if ((int)data.r.controller == fromNumber)
        data.r.controller = toNumber;
    else if ((int)data.r.controller == toNumber)
        data.r.controller = fromNumber;
}

void AtomRegister::shiftControllerNumbers(int number)
{
    if ((int)data.r.controller > number)
        data.r.controller --;
}

QDebug &operator<<(QDebug &out, const AtomRegister &ar) {
    out << ar.toString();
    return out;
}

QDebug &operator<<(QDebug &out, const RegisterList &rl) {
    for (auto& entry: rl)
        out << entry;
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
