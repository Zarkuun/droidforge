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

QDebug &operator<<(QDebug &out, const RegisterList &rl) {
    for (auto& entry: rl)
        out << entry;
    return out;
}
