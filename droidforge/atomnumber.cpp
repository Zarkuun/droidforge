#include "atomnumber.h"

QString AtomNumber::toString()
{
    // TODO: Formatierung gemäß Typ, Optimierung
    // bezüglich der Genauigkeit etc.
    return QString::number(number);
}
