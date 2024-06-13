#include "atomnumber.h"
#include "tuning.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)


AtomNumber *AtomNumber::clone() const
{
    return new AtomNumber(number, numberType);
}
QString AtomNumber::toString() const
{
    if (numberType == ATOM_NUMBER_FRACTION)
        return niceNumber(1.0 / number);

    double num = number;

    if (numberType == ATOM_NUMBER_ONOFF) {
        return number == 0 ? "off" : "on";
    }

    double factor = 1.0;
    QString suffix = "";

    if (numberType == ATOM_NUMBER_PERCENTAGE) {
        factor = 100;
        suffix = "%";
    }
    else if (numberType == ATOM_NUMBER_VOLTAGE) {
        factor = 10;
        suffix = "V";
    }
    return niceNumber(num * factor) + suffix;
}
QString AtomNumber::toCanonicalString() const
{
    return niceNumber(number);
}
QString AtomNumber::toDisplay() const
{
    if (numberType == ATOM_NUMBER_FRACTION)
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
    AtomNumber n(-number, numberType);
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
QString AtomNumber::niceNumber(double num)
{
    int l = num == 0 ? 0 : log10(abs(num));
    int precision = NUMBER_DIGITS - l;
    QString numString = QString::number(num, 'd', precision);
    while (numString.contains('.') && numString.endsWith('0'))
        numString.chop(1);
    if (numString.endsWith('.'))
        numString.chop(1);
    return numString;
}
QString AtomNumber::toFractionString(double number) const
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

    double maxError = FRACTION_PRECISION;

    while (true) {
        unsigned mid_p = left_p + right_p;
        unsigned mid_q = left_q + right_q;
        double mid = (double)mid_p / (double)mid_q;
        if (qAbs(number - mid) <= maxError) {
            if (reci)
                return QString("%1/%2").arg(mid_q).arg(niceNumber(mid_p));
            else
                return QString("%1/%2").arg(mid_p).arg(niceNumber(mid_q));
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
