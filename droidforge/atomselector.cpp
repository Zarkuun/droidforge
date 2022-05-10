#include "atomselector.h"
#include "atomnumber.h"
#include "numberselector.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

AtomSelector::AtomSelector(QWidget *parent)
    : QWidget{parent}
{
    int w = 150;
    numberSelector = new NumberSelector(this);
    numberSelector->setFixedWidth(150);
    inputOutputSelector = new InputOutputSelector(this);
    inputOutputSelector->setFixedWidth(150);
    QPushButton *controlSelector = new QPushButton("Controls", this);
    controlSelector->setFixedWidth(150);
    QPushButton *cableSelector = new QPushButton("Internal", this);
    cableSelector->setFixedWidth(150);

    QLabel *labelNumber = new QLabel(tr("Value"), this);
    QLabel *labelJack = new QLabel(tr("Input/Output"), this);
    QLabel *labelControl = new QLabel(tr("Control"), this);
    QLabel *labelCable = new QLabel(tr("Internal"), this);

    labelCable->setStyleSheet("border: 1px solid green;");


    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(labelNumber,     0, 0);
    layout->addWidget(hline(),         1, 0);
    layout->addWidget(numberSelector,  2, 0);

    layout->addWidget(vline(),         0, 1, 3, 1);

    layout->addWidget(labelJack,       0, 2);
    layout->addWidget(hline(),         1, 2);
    layout->addWidget(inputOutputSelector, 2, 2);

    layout->addWidget(vline(),         0, 3, 3, 1);

    layout->addWidget(labelControl,    0, 4);
    layout->addWidget(hline(),         1, 4);
    layout->addWidget(controlSelector, 2, 4);

    layout->addWidget(vline(),         0, 5, 3, 1);

    layout->addWidget(labelCable,      0, 6);
    layout->addWidget(hline(),         1, 6);
    layout->addWidget(cableSelector,   2, 6);

    setLayout(layout);

}

void AtomSelector::setAtom(const Atom *atom)
{
    numberSelector->clearAtom();
    if (!atom)
        return;

    if (atom->isNumber())
        numberSelector->setAtom((AtomNumber *)atom);
}

Atom *AtomSelector::getAtom()
{
    // Hier müssen wir rausfinden, welcher Typ gerade gewählt ist
    return numberSelector->getAtom();
}


QWidget *AtomSelector::vline() const
{
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setLineWidth(0);
    // TODO: Stylen
    return line;

}


QWidget *AtomSelector::hline() const
{
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setLineWidth(1);
    return line;
}
