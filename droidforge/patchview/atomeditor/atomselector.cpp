#include "atomselector.h"
#include "atomnumber.h"
#include "atomregister.h"
#include "cableselector.h"
#include "colorscheme.h"
#include "controlselector.h"
#include "globals.h"
#include "numberselector.h"
#include "inputoutputselector.h"
#include "droidfirmware.h"
#include "iconbase.h"
#include "tuning.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QLabel>
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>

AtomSelector::AtomSelector(jacktype_t jacktype, QWidget *parent)
    : QWidget{parent}
    , currentSelector(0)
    , numberSelector(0)
    , jackType(jacktype)
{
    if (jacktype == JACKTYPE_INPUT) {
        numberSelector = new NumberSelector(this);
        subSelectors.append(numberSelector);
    }
    subSelectors.append(new InputOutputSelector(jacktype, this));
    subSelectors.append(new ControlSelector(jacktype, this));
    CableSelector *cs = new CableSelector(this);
    subSelectors.append(cs);
    connect(cs, &CableSelector::committed, this, &AtomSelector::commit);

    auto mainLayout = new QVBoxLayout(this);
    auto selectorLayout = new QGridLayout; // (mainLayout);
    for (qsizetype i=0; i<subSelectors.count(); i++) {
        AtomSubSelector *ss = subSelectors[i];
        connect(ss, &AtomSubSelector::gotSelected, this, &AtomSelector::subselectorSelected);
        ss->installFocusFilter(ss);
        QPushButton *button = new QPushButton(ss->title());
        connect(button, &QPushButton::pressed, this, [this, i]() { this->switchToSelector(i); });
        selectorLayout->addWidget(button, 0, i);
        selectorLayout->addWidget(ss, 1, i);
    }

    setLayout(mainLayout);
    mainLayout->addLayout(selectorLayout);

    // Description
    labelDescription = new QLabel(this);
    labelDescription->setWordWrap(true);
    labelDescription->setTextFormat(Qt::RichText);
    labelDescription->setStyleSheet(QString("QLabel { padding: 10px; }"));
    labelDescription->setAlignment(Qt::AlignTop);
    labelDescription->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelDescription->setFocusPolicy(Qt::NoFocus);

    scrollArea = new QScrollArea();
    scrollArea->setWidget(labelDescription);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFocusPolicy(Qt::NoFocus);
    mainLayout->addWidget(scrollArea);

    // Jack type
    QHBoxLayout *typeLayout = new QHBoxLayout();
    labelJackType = new EllipsisLabel(this);
    labelJackType->setMinimumWidth(100);
    labelJackType->setTextInteractionFlags(Qt::TextSelectableByMouse);

    labelJackTypeIcon = new QLabel(this);
    mainLayout->addWidget(labelJackTypeIcon);

    typeLayout->addWidget(labelJackTypeIcon);
    typeLayout->addWidget(labelJackType);
    typeLayout->setStretch(0, 0);
    typeLayout->setStretch(1, 1);
    mainLayout->addLayout(typeLayout);

    mainLayout->setStretch(0, 10); // Atom selectors
    mainLayout->setStretch(1, 7);  // Description
    mainLayout->setStretch(1, 0);  // Jack type
}
void AtomSelector::setAllowFraction(bool af)
{
    if (numberSelector)
        numberSelector->setAllowFraction(af);
}
void AtomSelector::setCircuitAndJack(QString circuit, QString jack)
{
    if (numberSelector)
        numberSelector->setCircuitAndJack(circuit, jack);

    QString whence = jackType == JACKTYPE_INPUT ? "inputs" : "outputs";
    QString desc = the_firmware->jackDescriptionHTML(circuit, whence, jack);
    labelDescription->setText(desc);
    labelJackType->setText(the_firmware->jackTypeDescriptionHTML(circuit, whence, jack));
    // auto icon = IconBase::jackTypeSymbol("cv");
    QString jackType = the_firmware->jackTypeSymbol(circuit, whence, jack);
    QPixmap jackTypeSymbol(QString(JACK_TYPE_SYMBOLS_PATH_TEMPLATE)
               .arg(the_colorscheme->isDark() ? "dark" : "light",
                    jackType));

    labelJackTypeIcon->setPixmap(jackTypeSymbol.scaledToWidth(ASEL_JACKTYPE_WIDTH, Qt::SmoothTransformation));
    scrollArea->verticalScrollBar()->adjustSize();
}
void AtomSelector::setPatch(const Patch *patch)
{
    for (auto ss: subSelectors)
        ss->setPatch(patch);
}
void AtomSelector::setAtom(const Patch *patch, const Atom *atom)
{
    if (!atom) {
        switchToSelector(0);
        for (auto ss: subSelectors)
            ss->clearAtom();
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
void AtomSelector::commit()
{
    emit comitted();
}
