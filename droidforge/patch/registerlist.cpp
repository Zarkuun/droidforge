#include "registerlist.h"
#include "atomregister.h"

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

QDebug &operator<<(QDebug &out, const RegisterList &pos) {
    for (auto& entry: pos)
        out << entry;
    return out;
}
