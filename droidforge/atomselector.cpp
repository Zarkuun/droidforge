#include "atomselector.h"
#include "atomnumber.h"
#include "atomregister.h"
#include "cableselector.h"
#include "controlselector.h"
#include "numberselector.h"
#include "inputoutputselector.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

AtomSelector::AtomSelector(QWidget *parent)
    : QWidget{parent}
    , currentSelector(0)
{
    subSelectors.append(new NumberSelector(this));
    subSelectors.append(new InputOutputSelector(this));
    subSelectors.append(new ControlSelector(this));
    subSelectors.append(new CableSelector(this));

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        QPushButton *button = new QPushButton(ss->title());
        connect(button, &QPushButton::pressed, this, [this, i]() { this->switchToSelector(i); });
        layout->addWidget(button, 0, i);
        layout->addWidget(ss, 1, i);
    }
}

void AtomSelector::setAtom(const Patch *patch, const Atom *atom)
{
    if (!atom) {
        switchToSelector(0);
        return;
    }

    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        if (ss->handlesAtom(atom)) {
            ss->setAtom(patch, atom);
            switchToSelector(i);
            ss->setEnabled(true);
        }
        else {
            subSelectors[i]->clearAtom();
            ss->setEnabled(false);
        }
    }
}

Atom *AtomSelector::getAtom()
{
    return subSelectors[currentSelector]->getAtom();
}

void AtomSelector::switchToSelector(int index)
{
    currentSelector = index;
    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        ss->setEnabled(i == currentSelector);
        if (i == currentSelector)
            ss->getFocus();
    }
}
