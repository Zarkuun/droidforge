#include "inputoutputselector.h"
#include "atomregister.h"

#include <QHBoxLayout>
#include <QPalette>
#include <QKeyEvent>

InputOutputSelector::InputOutputSelector(jacktype_t jacktype, QWidget *parent)
    : RegisterSelector(
          false,
          jacktype == JACKTYPE_INPUT ? REGISTER_INPUT : REGISTER_OUTPUT,
          jacktype == JACKTYPE_INPUT ? "IGRO" : "OGNRX",
          parent)
    , jacktype(jacktype)
{
    if (jacktype == JACKTYPE_INPUT) {
        addRegisterButton(REGISTER_INPUT, tr("Input"));
        addRegisterButton(REGISTER_GATE, tr("Gate"));
        addRegisterButton(REGISTER_RGB, tr("RGB-LED"));
        addRegisterButton(REGISTER_OUTPUT, tr("Output"));
    }
    else {
        addRegisterButton(REGISTER_OUTPUT, tr("Output"));
        addRegisterButton(REGISTER_GATE, tr("Gate"));
        addRegisterButton(REGISTER_NORMALIZE, tr("Normalization"));
        addRegisterButton(REGISTER_RGB, tr("RGB-LED"));
        addRegisterButton(REGISTER_EXTRA, tr("Special"));
    }
}

QString InputOutputSelector::title() const
{
    if (jacktype == JACKTYPE_INPUT)
        return QString("External input");
    else
        return QString("External output");
}

bool InputOutputSelector::handlesAtom(const Atom *atom) const
{
    if (atom->isInvalid()) {
        QString s = atom->toString();
        if (s.isEmpty() || !s[0].isDigit())
            return true;
        else
            return false;
    }
    else
        return atom->isRegister() &&
                !((AtomRegister *)atom)->isControl();

}