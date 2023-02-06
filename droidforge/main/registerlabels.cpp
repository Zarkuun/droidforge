#include "registerlabels.h"
#include "globals.h"
#include "tuning.h"
#include "atomregister.h"

#include <QDebug>


QString RegisterLabels::toString() const
{
    QString s;

    typedef struct {
        char reg;
        const char *title;
    } regtitle_t;


    // First output global registers
    static regtitle_t globalRegtypes[] =  {
        { REGISTER_INPUT, "INPUTS" },
        { REGISTER_NORMALIZE, "NORMALIZATIONS" },
        { REGISTER_OUTPUT, "OUTPUTS" },
        { REGISTER_GATE, "GATES" },
        { REGISTER_RGB_LED, "RGB LEDS" },
    };

    for (unsigned i=0; i<sizeof(globalRegtypes) / sizeof(regtitle_t); i++)
        s += toString(globalRegtypes[i].reg, 0, globalRegtypes[i].title);

    // And now the registers on the controllers
    static regtitle_t controllerRegtypes[] =  {
        { REGISTER_POT, "POTS" },
        { REGISTER_BUTTON, "BUTTONS" },
        { REGISTER_LED, "LEDS" },
        { REGISTER_SWITCH, "SWITCHES" },
        { REGISTER_RGB_LED, "RGB LEDS" },
    };

    for (unsigned cn=0; cn<MAX_NUM_CONTROLLERS; cn++) {
        QString sc;
        for (unsigned j=0; j<sizeof(controllerRegtypes) / sizeof(regtitle_t); j++)
            sc += toString(controllerRegtypes[j].reg, cn+1);
        if (!sc.isEmpty()) {
            s += "# CONTROLLER " + QString::number(cn+1) + ":\n";
            s += sc;
            s += "\n";
        }
    }
    return s;
}

void RegisterLabels::remapRegister(AtomRegister from, AtomRegister to)
{
    if (contains(from)) {
        (*this)[to] = (*this)[from];
        remove(from);
    }
}
void RegisterLabels::swapRegisters(AtomRegister regA, AtomRegister regB)
{
    RegisterLabel labA, labB;
    bool hadA = false;
    bool hadB = false;

    if (contains(regA)) {
        hadA = true;
        labA = value(regA);
        remove(regA);
    }
    if (contains(regB)) {
        hadB = true;
        labB = value(regB);
        remove(regB);
    }

    if (hadA)
        insert(regB, labA);
    if (hadB)
        insert(regA, labB);
}
void RegisterLabels::swapControllerNumbers(unsigned fromnum, unsigned tonum)
{
    QMap<AtomRegister, RegisterLabel> swapped;

    for (auto it = keyBegin(); it != keyEnd(); ++it)
    {
        AtomRegister reg = *it;
        if (!reg.isControl())
            swapped[reg] = (*this)[reg];
        else {
            AtomRegister newReg;
            if (reg.controller() == fromnum)
                newReg = AtomRegister(reg.getRegisterType(), tonum, reg.number());
            else if (reg.controller() == tonum)
                newReg = AtomRegister(reg.getRegisterType(), fromnum, reg.number());
            else
                newReg = reg;
            swapped[newReg] = (*this)[reg];
        }
    }

    clear();
    for (auto it = swapped.keyBegin(); it != swapped.keyEnd(); ++it)
        (*this)[*it] = swapped[*it];
}

void RegisterLabels::shiftControllerNumbers(int number, int by)
{
    QMap<AtomRegister, RegisterLabel> shifted;

    for (auto it = keyBegin(); it != keyEnd(); ++it)
    {
        AtomRegister reg = *it;
        RegisterLabel label = (*this)[reg];
        reg.shiftControllerNumbers(number, by);
        shifted[reg] = label;
    }
    clear();
    for (auto it = shifted.keyBegin(); it != shifted.keyEnd(); ++it)
        (*this)[*it] = shifted[*it];
}

void RegisterLabels::removeController(int number)
{
    QMap<AtomRegister, RegisterLabel> removed;
    for (auto it = keyBegin(); it != keyEnd(); ++it)
    {
        AtomRegister reg = *it;
        if (reg.controller() != (unsigned)number)
            removed[reg] = (*this)[reg];
    }
    clear();
    for (auto it = removed.keyBegin(); it != removed.keyEnd(); ++it)
        (*this)[*it] = removed[*it];
}
void RegisterLabels::copyControllerLabels(int fromNumber, int toNumber)
{
    shout << "Copiing" << fromNumber << "to" << toNumber;
    QMapIterator<AtomRegister, RegisterLabel> it(*this);
    while (it.hasNext()) {
        it.next();
        AtomRegister atom = it.key();
        const RegisterLabel &label = it.value();
        shout << "found label at" << atom << atom.controller();
        if (atom.controller() == (unsigned)fromNumber) {
            AtomRegister copied(atom.getRegisterType(), toNumber, atom.getNumber());
            (*this)[copied] = label;
            shout << "COPIED" << copied << "TO" << label.shorthand << label.description;
        }
    }
}
QString RegisterLabels::toString(char reg, unsigned controller, const QString &title) const
{
    QString s;
    bool first = true;

    QMapIterator<AtomRegister, RegisterLabel> it(*this);
    while (it.hasNext()) {
        it.next();
        AtomRegister atom = it.key();
        const RegisterLabel &label = it.value();
        if (atom.getRegisterType() == reg && atom.controller() == controller) {
            if (first) {
                first = false;
                if (title != "")
                    s += QString("# ") + title + ":\n";
            }
            s += "#  " + atom.toString() + ": ";
            if (!label.shorthand.isEmpty()) {
                s += "[" + label.shorthand + "]";
                if (label.description != "")
                    s += " ";
            }
            if (label.description != "")
                s += label.description;
            s += "\n";
        }
    }
    if (title != "" && s != "")
        s += "\n";
    return s;
}
