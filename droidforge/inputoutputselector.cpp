#include "inputoutputselector.h"
#include "atomregister.h"

#include <QHBoxLayout>
#include <QPalette>
#include <QKeyEvent>

InputOutputSelector::InputOutputSelector(jacktype_t jacktype, QWidget *parent)
    : RegisterSelector(
          false,
          jacktype == JACKTYPE_INPUT ? 'I' : 'O',
          jacktype == JACKTYPE_INPUT ? "IGRO" : "OGNRX",
          parent)
    , jacktype(jacktype)
{
    if (jacktype == JACKTYPE_INPUT) {
        addRegisterButton('I', tr("Input"));
        addRegisterButton('G', tr("Gate"));
        addRegisterButton('R', tr("RGB-LED"));
        addRegisterButton('O', tr("Output"));
    }
    else {
        addRegisterButton('O', tr("Output"));
        addRegisterButton('G', tr("Gate"));
        addRegisterButton('N', tr("Normalization"));
        addRegisterButton('R', tr("RGB-LED"));
        addRegisterButton('X', tr("Special"));
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
