#include "cableselector.h"

#include <QVBoxLayout>

CableSelector::CableSelector(QWidget *parent)
    : QGroupBox{parent}
{
    comboBox = new QComboBox(this);
    comboBox->setEditable(true);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(comboBox);
    mainLayout->addStretch();
}

void CableSelector::setAtom(const Patch *patch, AtomCable *ac)
{
    cable = ac->getCable();
    comboBox->clear();
    QStringList cables = patch->allCables();
    qDebug() << "CABLES" << cables;
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
