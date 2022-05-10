#include "atomselector.h"
#include "atomnumber.h"
#include "numberselector.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

AtomSelector::AtomSelector(QWidget *parent)
    : QWidget{parent}
{
    numberSelector = new NumberSelector(this);
    QPushButton *jackSelector = new QPushButton("Jack", this);
    QPushButton *controlSelector = new QPushButton("Controls", this);
    QPushButton *cableSelector = new QPushButton("Internal", this);

    QLabel *labelValue = new QLabel(tr("Value"), this);
    QLabel *labelJack = new QLabel(tr("Input/Output"), this);
    QLabel *labelControl = new QLabel(tr("Control"), this);
    QLabel *labelCable = new QLabel(tr("Internal"), this);

    labelCable->setStyleSheet("border: 1px solid green;");

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(labelValue, 0, 0);
    layout->addWidget(labelJack, 0, 1);
    layout->addWidget(labelControl, 0, 2);
    layout->addWidget(labelCable, 0, 3);

    layout->addWidget(numberSelector, 1, 0);
    layout->addWidget(jackSelector, 1, 1);
    layout->addWidget(controlSelector, 1, 2);
    layout->addWidget(cableSelector, 1, 3);

    setLayout(layout);

}

void AtomSelector::setAtom(const Atom *atom)
{
    if (atom->isNumber())
        numberSelector->setAtom((AtomNumber *)atom);
}

Atom *AtomSelector::getAtom()
{
    // Hier müssen wir rausfinden, welcher Typ gerade gewählt ist
    return numberSelector->getAtom();
}
