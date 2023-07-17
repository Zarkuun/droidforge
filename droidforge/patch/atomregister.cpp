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

AtomRegister::AtomRegister(const AtomRegister &ar)
{
    registerType = ar.registerType;
    cont = ar.cont;
    num = ar.num;
}
AtomRegister::AtomRegister(const QString &s)
{
    // Note: we allow invalid registers such as I0 here. It's easer
    // for creating precise error messages later.
    static QRegularExpression expa("^([INOGRX])([0-9]+)$", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression expb("^([BLPSR])([0-9]+)[.]([0-9]+)$", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m;

    m = expa.match(s);
    if (m.hasMatch()) {
        registerType = m.captured(1).toUpper()[0].toLatin1();
        num = m.captured(2).toUInt();
        cont = 0;
        return;
    }

    m = expb.match(s);
    if (m.hasMatch()) {
        registerType = m.captured(1).toUpper()[0].toLatin1();
        cont = m.captured(2).toUInt();
        num = m.captured(3).toUInt();
    }
    else {
        registerType = 0;
        cont = 0;
        num = 0;
    }
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
bool AtomRegister::isNull() const
{
    return registerType == 0;
}
bool AtomRegister::canHaveLabel() const
{
    return registerType == REGISTER_BUTTON
             || registerType == REGISTER_POT
             || registerType == REGISTER_LED
             || registerType == REGISTER_SWITCH
             || registerType == REGISTER_INPUT
             || registerType == REGISTER_NORMALIZE
             || registerType == REGISTER_OUTPUT
            || registerType == REGISTER_GATE;
}
AtomRegister AtomRegister::relatedRegisterWithLabel() const
{
    if (registerType == REGISTER_NORMALIZE)
        return AtomRegister(REGISTER_INPUT, cont, num);
    else if (registerType == REGISTER_LED)
        return AtomRegister(REGISTER_BUTTON, cont, num);
    else
        return *this;
}
bool AtomRegister::isRelatedTo(const AtomRegister &other) const
{
    if (cont != other.cont)
        return false;
    else if (num != other.num)
        return false;
    else if (registerType == other.registerType) // identical
        return true;

    // Buttons and their LEDs (and in the M4 also the RGB registers)
    else if ((registerType == REGISTER_BUTTON
             || registerType == REGISTER_LED
             || registerType == REGISTER_RGB_LED) &&
            (other.registerType == REGISTER_BUTTON
                  || other.registerType == REGISTER_LED
                  || other.registerType == REGISTER_RGB_LED) )
        return true;


    // Inputs of the master and their normalizations
    else if (registerType == REGISTER_INPUT && other.registerType == REGISTER_NORMALIZE)
        return true;
    else if (registerType == REGISTER_NORMALIZE && other.registerType == REGISTER_INPUT)
        return true;

    else
        return false;
}
unsigned AtomRegister::neededG8Number() const
{
    if (registerType == REGISTER_GATE
           && num >= 1
           && num <= 8)
        return cont;

    if (registerType == REGISTER_RGB_LED
            && cont == 0
            && num >= 17
            && num <= 48)
        return (num - 16) / 8;

    return 0;
}
bool AtomRegister::needsX7() const
{
    if (registerType == REGISTER_GATE
           && cont == 0
           && num >= 9
           && num <= 12)
        return true;

    if (registerType == REGISTER_RGB_LED
            && cont == 0
            && num >= 25
            && num <= 32)
        return true;

    return false;
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
    if ((int)cont > controller)
        cont += by;
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
void AtomRegister::incrementForExpansion(const Patch *patch)
{
    num++;
    if (!patch->registerAvailable(*this))
        num--;
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
    if (a.cont < b.cont)
        return true;
    else if (a.cont > b.cont)
        return false;
    else if (a.registerType < b.registerType)
        return true;
    else if (a.registerType > b.registerType)
        return false;
    else return a.num < b.num;
}
