#include "registerlist.h"
#include "atomregister.h"
#include "globals.h"

RegisterList::RegisterList()
{
}

QString RegisterList::toString() const
{
    QString result;
    for (auto &reg: *this) {
        if (!result.isEmpty())
            result += " ";
        result += reg.toString();
    }
    return result;
}

QString RegisterList::toSmartString() const
{
    if (isEmpty())
        return "";

    // I assume that we are sorted.
    QString result;
    AtomRegister previous;
    unsigned groupLength = 0;

    for (auto &reg: *this) {
        if (reg.getRegisterType() == previous.getRegisterType()
            && reg.controller() == previous.controller()
            && reg.number() == previous.number() + 1)
        {
            groupLength ++;
            previous = reg;
            continue;
        }
        else {
            if (groupLength > 0) {
                if (groupLength > 1)
                    result += " ... ";
                else
                    result += ", ";
                result += previous.toString();
            }
            groupLength = 0;
            result += ", " + reg.toString();
        }
        previous = reg;
    }
    if (groupLength > 0) {
        if (groupLength > 1)
            result += " ... ";
        else
            result += ", ";
        result += previous.toString();
    }
    return result.mid(2); // skip first ", "
}

QDebug &operator<<(QDebug &out, const RegisterList &pos) {
    for (auto& entry: pos)
        out << entry;
    return out;
}
