#include "cableselector.h"
#include "globals.h"
#include "tuning.h"
#include "cablecolorizer.h"

#include <QGridLayout>

CableSelector::CableSelector(QWidget *parent)
    : AtomSubSelector{parent}
{
    setMinimumWidth(ASEL_SUBSELECTOR_WIDTH);
    setMaximumWidth(QWIDGETSIZE_MAX);

    QGridLayout *mainLayout = new QGridLayout(this);

    // Icon for current cable
    labelIcon = new QLabel();
    mainLayout->addWidget(labelIcon, 0, 0);

    // Line edit for creating new cables
    static QRegularExpression re("[_0-9a-zA-Z]+");
    lineEdit = new CableSelectorLineEdit(this);
    lineEdit->setValidator(new QRegularExpressionValidator(re, this));
    mainLayout->addWidget(lineEdit, 0, 1);

    // List for selecting existing cables
    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget, 1, 0, 1, 2);

    connect(lineEdit, &QLineEdit::textEdited, this, &CableSelector::cableEdited);
    connect(lineEdit, &CableSelectorLineEdit::keyPressed, this, &CableSelector::lineKeyPressed);
    connect(listWidget, &QListWidget::currentRowChanged, this, &CableSelector::cableSelected);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &CableSelector::itemDoubleClicked);
}
bool CableSelector::handlesAtom(const Atom *atom) const
{
    return atom->isCable();
}
void CableSelector::setPatch(const Patch *newPatch)
{
    patch = newPatch;
    updateList();
}
void CableSelector::updateList()
{
    QString filter;
    if (!lineEdit->hasSelectedText())
        filter = lineEdit->text().toUpper();

    listWidget->clear();
    QStringList cables = patch->allCables();
    for (auto &cable: cables) {
        if (cable.contains(filter)) {
            const QIcon *icon = the_cable_colorizer->iconForCable(cable);
            QListWidgetItem *item = new QListWidgetItem(*icon, cable, listWidget);
            listWidget->addItem(item);
        }
    }
}
void CableSelector::setAtom(const Patch *patch, const Atom *atom)
{
    cable = ((const AtomCable *)atom)->getCable();
    QStringList cables = patch->allCables();
    int index = cables.indexOf(cable);
    listWidget->setCurrentRow(index);
}
void CableSelector::clearAtom()
{
    lineEdit->setText("");
    cable = "";
}
Atom *CableSelector::getAtom() const
{
    return new AtomCable(lineEdit->text());
}
void CableSelector::getFocus()
{
    lineEdit->setFocus();
}
void CableSelector::installFocusFilter(QWidget *w)
{
    lineEdit->installEventFilter(w);
    listWidget->installEventFilter(w);
}
void CableSelector::cableEdited(QString text)
{
    if (text != text.toUpper())
        lineEdit->setText(text.toUpper());
    updateIcon();
    updateList();
}
void CableSelector::cableSelected(int row)
{
    if (row >= 0) {
        lineEdit->setText(listWidget->item(row)->text());
        updateIcon();
    }
}
void CableSelector::itemDoubleClicked()
{
    emit committed();
}
void CableSelector::updateIcon()
{
    const QImage *image = the_cable_colorizer->imageForCable(lineEdit->text());
    QImage scaled = *image;
    scaled = scaled.scaled(59, 14);
    labelIcon->setPixmap(QPixmap::fromImage(scaled));
}
void CableSelector::lineKeyPressed(int key)
{
    if (key == Qt::Key_Down) {
        listWidget->setFocus();
        listWidget->setCurrentRow(0);
    }
}
