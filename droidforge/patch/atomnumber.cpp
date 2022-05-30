#include "atomnumber.h"
#include "tuning.h"
#include "patchproblem.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)


AtomNumber *AtomNumber::clone() const
{
    return new AtomNumber(number, numberType, fraction);
}

QString AtomNumber::toString() const
{
    float num = fraction ? 1.0 / number : number;

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
    return QString::number(num * factor) + suffix;
}

QString AtomNumber::toDisplay() const
{
    if (fraction)
        return "1 / " + toString();
    else
        return toString();
}

bool AtomNumber::isNegatable() const
{
    return number < 0 && numberType != ATOM_NUMBER_ONOFF;
}

QString AtomNumber::toNegatedString() const
{
    AtomNumber n(-number, numberType, fraction);
    return n.toString();
}

QString AtomNumber::problemAsInput(const Patch *) const
{
    return "";
}

QString AtomNumber::problemAsOutput(const Patch *) const
{
    return tr("You cannot use a fixed number for an output parameter");
}

QString AtomNumber::toFractionString(float number) const
{
    if (number < 0)
        return "-" + toFractionString(-number);

    bool reci = false;
    if (number > 1.0) {
        reci = true;
        number = 1.0 / number;
    }

    unsigned left_p = 0;
    unsigned left_q = 1;
    unsigned right_p = 1;
    unsigned right_q = 1;

    float maxError = FRACTION_PRECISION;

    while (true) {
        unsigned mid_p = left_p + right_p;
        unsigned mid_q = left_q + right_q;
        float mid = (float)mid_p / (float)mid_q;
        if (qAbs(number - mid) <= maxError) {
            if (reci)
                return QString("%1/%2").arg(mid_q).arg(mid_p);
            else
                return QString("%1/%2").arg(mid_p).arg(mid_q);
        }
        else if (number > mid) {
            left_p = mid_p;
            left_q = mid_q;
        }
        else {
            right_p = mid_p;
            right_q = mid_q;
        }
    }
}
