#include "atomselector.h"
#include "atomnumber.h"
#include "atomregister.h"
#include "cableselector.h"
#include "controlselector.h"
#include "numberselector.h"
#include "inputoutputselector.h"

#include <QGridLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QLabel>
#include <QTimer>

AtomSelector::AtomSelector(jacktype_t jacktype, QWidget *parent)
    : QWidget{parent}
    , currentSelector(0)
{
    if (jacktype == JACKTYPE_INPUT)
        subSelectors.append(new NumberSelector(this));
    subSelectors.append(new InputOutputSelector(jacktype, this));
    subSelectors.append(new ControlSelector(jacktype, this));
    subSelectors.append(new CableSelector(this));

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        connect(ss, &AtomSubSelector::gotSelected, this, &AtomSelector::subselectorSelected);
        ss->installFocusFilter(ss);
        QPushButton *button = new QPushButton(ss->title());
        connect(button, &QPushButton::pressed, this, [this, i]() { this->switchToSelector(i); });
        layout->addWidget(button, 0, i);
        layout->addWidget(ss, 1, i);
    }

    // Dieses Widget kann Focus bekommen, und zwar nur durch
    // die Tabtaste.
    // setFocusPolicy(Qt::TabFocus);
    // setFocusPolicy(Qt::StrongFocus);
}

void AtomSelector::setAtom(const Patch *patch, const Atom *atom)
{
    if (!atom) {
        switchToSelector(0);
        return;
    }

    int sel = 0;
    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        if (ss->handlesAtom(atom)) {
            ss->setAtom(patch, atom);
            sel = i;
        }
        else
            subSelectors[i]->clearAtom();
    }
    switchToSelector(sel);
}

Atom *AtomSelector::getAtom()
{
    if (currentSelector)
        return currentSelector->getAtom();
    else
        return 0; // should never happen
}

void AtomSelector::mousePressEvent(QMouseEvent *event)
{
    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        QRect geo = ss->geometry();
        if (geo.contains(event->pos())) {
            switchToSelector(i);
            event->accept();
            return;
        }
    }
    event->ignore();
}

void AtomSelector::subselectorSelected(AtomSubSelector *ass)
{
     currentSelector = ass;
}

void AtomSelector::switchToSelector(int index)
{
    subSelectors[index]->getFocus();
}