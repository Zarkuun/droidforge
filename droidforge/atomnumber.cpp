#include "atomnumber.h"

AtomNumber *AtomNumber::clone() const
{
    return new AtomNumber(number, numberType);
}


QString AtomNumber::toString()
{
    // TODO: Formatierung gemäß Typ, Optimierung
    // bezüglich der Genauigkeit etc.
    if (numberType == ATOM_NUMBER_ONOFF) {
        return number == 0 ? "off" : "on";
    }

    float factor = 1.0;
    QString suffix = "";

    if (numberType == ATOM_NUMBER_PERCENTAGE) {
        factor = 100;
        suffix = "%";
    }
    else if (numberType == ATOM_NUMBER_VOLTAGE) {
        factor = 10;
        suffix = "V";
    }

    return QString::number(number * factor) + suffix;
}
