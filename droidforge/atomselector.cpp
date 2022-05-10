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
    QWidget *controlSelector = new QWidget(this);
    controlSelector->setFixedWidth(w);
    QWidget *cableSelector = new QWidget(this);
    cableSelector->setFixedWidth(w);

    // QLabel *labelNumber = new QLabel(tr("Value"), this);
    // QLabel *labelJack = new QLabel(tr("Input/Output"), this);
    // QLabel *labelControl = new QLabel(tr("Control"), this);
    // QLabel *labelCable = new QLabel(tr("Internal"), this);

    // labelCable->setStyleSheet("border: 1px solid green;");

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

}

void AtomSelector::setAtom(const Atom *atom)
{
    numberSelector->clearAtom();
    if (!atom)
        return;

    if (atom->isNumber()) {
        numberSelector->setAtom((AtomNumber *)atom);
        setSelectType(SELECT_NUMBER);
    }
    else if (atom->isRegister()) {
        AtomRegister *areg = (AtomRegister *)atom;
        if (areg->isControl()) {
            setSelectType(SELECT_CONTROL);
        }
        else {
            setSelectType(SELECT_INPUT_OUTPUT);
            inputOutputSelector->setAtom(areg);
        }
    }
    else if (atom->isCable()) {
        setSelectType(SELECT_CABLE);
    }
    else // empty
        setSelectType(SELECT_NUMBER);
}

Atom *AtomSelector::getAtom()
{
    qDebug() << "SEL" << selectType;
    switch (selectType) {
    case SELECT_NUMBER:
        return numberSelector->getAtom();
    case SELECT_INPUT_OUTPUT:
        return inputOutputSelector->getAtom();
    case SELECT_CONTROL:
        return 0; // TODO
    case SELECT_CABLE:
        return 0; // TODO
    }
}

void AtomSelector::setSelectType(select_t sel)
{
    selectType = sel;
    numberSelector->setDisabled(sel != SELECT_NUMBER);
    inputOutputSelector->setDisabled(sel != SELECT_INPUT_OUTPUT);
}
