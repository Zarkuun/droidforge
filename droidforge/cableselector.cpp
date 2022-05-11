#include "cableselector.h"

#include <QVBoxLayout>

CableSelector::CableSelector(QWidget *parent)
    : QGroupBox{parent}
{
    static QRegularExpression re("[a-zA-Z][_0-9a-zA-Z]*");

    comboBox = new QComboBox(this);
    comboBox->setEditable(true);
    comboBox->setValidator(new QRegularExpressionValidator(re, this));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(comboBox);
    mainLayout->addStretch();
    connect(comboBox, &QComboBox::editTextChanged, this, &CableSelector::cableEdited);
}

void CableSelector::setAtom(const Patch *patch, AtomCable *ac)
{
    cable = ac->getCable();
    comboBox->clear();
    QStringList cables = patch->allCables();
    comboBox->addItems(cables);
    comboBox->setCurrentText(cable);
}

void CableSelector::clearAtom()
{
    comboBox->setCurrentText("");
    cable = "";
}

AtomCable *CableSelector::getAtom()
{
    return new AtomCable(comboBox->currentText());
}

void CableSelector::getFocus()
{
    comboBox->setFocus();
}

void CableSelector::cableEdited(QString text)
{
    if (text != text.toUpper())
        comboBox->setCurrentText(text.toUpper());
}
