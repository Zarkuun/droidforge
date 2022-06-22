#include "cableselector.h"
#include "tuning.h"
#include "cablecolorizer.h"

#include <QVBoxLayout>

CableSelector::CableSelector(QWidget *parent)
    : AtomSubSelector{parent}
{
    setFixedWidth(2 * ASEL_SUBSELECTOR_WIDTH);

    static QRegularExpression re("[a-zA-Z][_0-9a-zA-Z]*");
    comboBox = new QComboBox(this);
    comboBox->setEditable(true);
    // comboBox->setValidator(new QRegularExpressionValidator(re, this));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(comboBox);
    mainLayout->addStretch();
    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget);
    connect(comboBox, &QComboBox::editTextChanged, this, &CableSelector::cableEdited);
}

bool CableSelector::handlesAtom(const Atom *atom) const
{
    return atom->isCable();
}

void CableSelector::setAtom(const Patch *patch, const Atom *atom)
{
    cable = ((const AtomCable *)atom)->getCable();
    comboBox->clear();
    QStringList cables = patch->allCables();
    for (auto &cable: cables) {
        comboBox->addItem(cable);
        const QIcon *icon = the_cable_colorizer->iconForCable(cable);
        QListWidgetItem *item = new QListWidgetItem(*icon, cable, listWidget);
        listWidget->addItem(item);
    }

    int index = cables.indexOf(cable);
    comboBox->setCurrentIndex(index);
}

void CableSelector::clearAtom()
{
    comboBox->setCurrentText("");
    cable = "";
}

Atom *CableSelector::getAtom() const
{
    return new AtomCable(comboBox->currentText());
}

void CableSelector::getFocus()
{
    comboBox->setFocus();
}

void CableSelector::installFocusFilter(QWidget *w)
{
    comboBox->installEventFilter(w);
}

void CableSelector::cableEdited(QString text)
{
    if (text != text.toUpper())
        comboBox->setCurrentText(text.toUpper());
}
