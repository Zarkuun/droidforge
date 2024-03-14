#include "atomregister.h"
#include "patch.h"

#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QRegularExpression>

#define tr(s) QCoreApplication::translate("Patch", s)

AtomRegister::AtomRegister()
    : registerType(0)
    , controller(0)
    , g8(0)
    , number(0)
{
}
AtomRegister::AtomRegister(char ty, unsigned co, unsigned g8, unsigned nr)
    : registerType(ty)
    , controller(co)
    , g8(g8)
    , number(nr)
{
}
AtomRegister::AtomRegister(const AtomRegister &ar)
{
    registerType = ar.registerType;
    controller = ar.controller;
    g8 = ar.g8;
    number = ar.number;
}
AtomRegister::AtomRegister(const QString &s)
{
    // Note: we allow invalid registers such as I0 here. It's easer
    // for creating precise error messages later.
    static QRegularExpression expa("^([INOGRX])([0-9]+)$", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression expb("^([GBLPESR])([0-9]+)[.]([0-9]+)$", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m;

    m = expa.match(s);
    if (m.hasMatch()) {
        registerType = m.captured(1).toUpper()[0].toLatin1();
        number = m.captured(2).toUInt();
        controller = 0;
        if (registerType == REGISTER_GATE && number >=1 && number <= 8)
            g8 = 1; // G5 -> G1.5
        else
            g8 = 0;
        return;
    }

    m = expb.match(s);
    if (m.hasMatch()) {
        registerType = m.captured(1).toUpper()[0].toLatin1();
        if (registerType == REGISTER_GATE) {
            g8 = m.captured(2).toUInt();
            controller = 0;
        }
        else {
            controller = m.captured(2).toUInt();
            g8 = 0;
        }
        number = m.captured(3).toUInt();
    }
    else {
        registerType = 0;
        controller = 0;
        g8 = 0;
        number = 0;
    }
}
AtomRegister AtomRegister::operator=(const AtomRegister &ar)
{
    registerType = ar.registerType;
    controller = ar.controller;
    g8 = ar.g8;
    number = ar.number;
    return *this;
}
AtomRegister *AtomRegister::clone() const
{
    return new AtomRegister(*this);
}
QString AtomRegister::toString() const
{
    if (controller)
        return registerType + QString::number(controller) + "." + QString::number(number);
    else if (g8)
        return registerType + QString::number(g8) + "." + QString::number(number);
    else
        return registerType + QString::number(number);
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
            || registerType == REGISTER_ENCODER
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
        return AtomRegister(REGISTER_INPUT, controller, 0, number);
    else if (registerType == REGISTER_LED)
        return AtomRegister(REGISTER_BUTTON, controller, 0, number);
    else
        return *this;
}
bool AtomRegister::isRelatedTo(const AtomRegister &other) const
{
    if (controller != other.controller)
        return false;
    else if (number != other.number)
        return false;
    else if (g8 != other.g8)
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

    // Buttons and LEDs are related to their encoders in E$
    else if ((registerType == REGISTER_BUTTON || registerType == REGISTER_LED)
             && other.registerType == REGISTER_ENCODER)
        return true;
    else if (registerType == REGISTER_ENCODER
             && (other.registerType == REGISTER_BUTTON || other.registerType == REGISTER_LED))
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
    if (g8)
        return g8;

    if (registerType == REGISTER_RGB_LED
            && controller == 0
            && number >= 17
            && number <= 48)
        return (number - 16) / 8;

    return 0;
}
bool AtomRegister::needsX7() const
{
    if (registerType == REGISTER_GATE
           && controller == 0
           && number >= 9
           && number <= 12)
        return true;

    if (registerType == REGISTER_RGB_LED
            && controller == 0
            && number >= 49
            && number <= 56)
        return true;

    return false;
}
void AtomRegister::swapControllerNumbers(int fromController, int toController)
{
    if ((int)controller == fromController)
        controller = toController;
    else if ((int)controller == toController)
        controller = fromController;
}
void AtomRegister::shiftControllerNumbers(int firstController, int by)
{
    if ((int)controller > firstController)
        controller += by;
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
        case REGISTER_ENCODER:
                return tr("You cannot use an encoder as output");
        case REGISTER_BUTTON:
                return tr("You cannot use a button as output");
        case REGISTER_SWITCH:
                return tr("You cannot use a switch as output");
    }
    return generalProblem(patch);
}
void AtomRegister::incrementForExpansion(const Patch *patch)
{
    number++;
    if (!patch->registerAvailable(*this))
        number--;
}
QString AtomRegister::generalProblem(const Patch *patch) const
{
    if (number <= 0)
        return tr("The number of the register may not be less than 1");
    else if (controller > patch->numControllers())
        return tr("Invalid controller number %1. You have just %2 controllers")
                .arg(controller).arg(patch->numControllers());
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
            && a.controller == b.controller
            && a.g8 == b.g8
            && a.number == b.number;
}
bool operator!=(const AtomRegister &a, const AtomRegister &b)
{
    return !(a == b);
}
bool operator<(const AtomRegister &a, const AtomRegister &b)
{
    if (a.controller < b.controller)
        return true;
    else if (a.controller > b.controller)
        return false;
    if (a.g8 < b.g8)
        return true;
    else if (a.g8 > b.g8)
        return false;
    else if (a.registerType < b.registerType)
        return true;
    else if (a.registerType > b.registerType)
        return false;
    else return a.number < b.number;
}
