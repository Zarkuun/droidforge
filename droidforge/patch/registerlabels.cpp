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
            s += "#   " + atom.toString() + ": ";
            if (!label.shorthand.isEmpty())
                s += "[" + label.shorthand + "] ";
            s += label.description;
            s += "\n";
        }
    }
    if (title != "" && s != "")
        s += "\n";
    return s;
}
