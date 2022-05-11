#include "atomselector.h"
#include "atomnumber.h"
#include "numberselector.h"
#include "atomregister.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

AtomSelector::AtomSelector(QWidget *parent)
    : QWidget{parent}
{
    int w = 150;
    numberSelector = new NumberSelector(this);
    numberSelector->setFixedWidth(w);
    inputOutputSelector = new InputOutputSelector(this);
    inputOutputSelector->setFixedWidth(w);
    controlSelector = new ControlSelector(this);
    controlSelector->setFixedWidth(w);
    cableSelector = new CableSelector(this);
    cableSelector->setFixedWidth(w);

    buttonNumber = new QPushButton(tr("Fixed number"));
    buttonInputOutput = new QPushButton(tr("Input / output"));
    buttonControl = new QPushButton(tr("Control"));
    buttonCable = new QPushButton(tr("Internal cable"));
    numberSelector->setDisabled(true);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(buttonNumber,        0, 0);
    layout->addWidget(numberSelector,      1, 0);
    layout->addWidget(buttonInputOutput,   0, 1);
    layout->addWidget(inputOutputSelector, 1, 1);
    layout->addWidget(buttonControl,       0, 2);
    layout->addWidget(controlSelector,     1, 2);
    layout->addWidget(buttonCable,         0, 3);
    layout->addWidget(cableSelector,       1, 3);

    setLayout(layout);

    connect(buttonNumber, &QPushButton::pressed, this, &AtomSelector::switchToNumber);
    connect(buttonInputOutput, &QPushButton::pressed, this, &AtomSelector::switchToInputOutput);
    connect(buttonControl, &QPushButton::pressed, this, &AtomSelector::switchToControl);
    connect(buttonCable, &QPushButton::pressed, this, &AtomSelector::switchToCable);
}


void AtomSelector::setAtom(const Patch *patch, const Atom *atom)
{
    numberSelector->clearAtom();
    inputOutputSelector->clearAtom();
    controlSelector->clearAtom();
    cableSelector->clearAtom();

    if (!atom) {
        setSelectType(SELECT_NUMBER);
        return;
    }

    if (atom->isNumber()) {
        numberSelector->setAtom((AtomNumber *)atom);
        setSelectType(SELECT_NUMBER);
    }
    else if (atom->isRegister()) {
        AtomRegister *areg = (AtomRegister *)atom;
        if (areg->isControl()) {
            setSelectType(SELECT_CONTROL);
            controlSelector->setAtom(areg);
        }
        else {
            setSelectType(SELECT_INPUT_OUTPUT);
            inputOutputSelector->setAtom(areg);
        }
    }
    else if (atom->isCable()) {
        cableSelector->setAtom(patch, (AtomCable *)atom);
        setSelectType(SELECT_CABLE);
    }
    else // empty
        setSelectType(SELECT_NUMBER);
}

Atom *AtomSelector::getAtom()
{
    switch (selectType) {
    case SELECT_NUMBER:
        return numberSelector->getAtom();
    case SELECT_INPUT_OUTPUT:
        return inputOutputSelector->getAtom();
    case SELECT_CONTROL:
        return controlSelector->getAtom();
    case SELECT_CABLE:
        return cableSelector->getAtom();
    }
}

void AtomSelector::setSelectType(select_t sel)
{
    selectType = sel;
    numberSelector->setDisabled(sel != SELECT_NUMBER);
    inputOutputSelector->setDisabled(sel != SELECT_INPUT_OUTPUT);
    controlSelector->setDisabled(sel != SELECT_CONTROL);
    cableSelector->setDisabled(sel != SELECT_CABLE);

    switch (selectType) {
    case SELECT_NUMBER:
        numberSelector->getFocus();
        break;

    case SELECT_INPUT_OUTPUT:
        inputOutputSelector->getFocus();
        break;

    case SELECT_CONTROL:
        controlSelector->getFocus();
        break;

    case SELECT_CABLE:
        cableSelector->getFocus();
        break;
    }

}

void AtomSelector::switchToNumber()
{
    setSelectType(SELECT_NUMBER);
}

void AtomSelector::switchToInputOutput()
{
    setSelectType(SELECT_INPUT_OUTPUT);
}

void AtomSelector::switchToControl()
{
    setSelectType(SELECT_CONTROL);
}

void AtomSelector::switchToCable()
{
    setSelectType(SELECT_CABLE);
}
