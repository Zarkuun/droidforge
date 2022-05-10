#include "inputoutputselector.h"

#include <QHBoxLayout>
#include <QPalette>

InputOutputSelector::InputOutputSelector(QWidget *parent)
    : QWidget{parent}
    , registerType('I')
{
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::green);
    setPalette(pal);

    labelRegister = new QLabel("H");
    lineEdit = new QLineEdit(this);
    QHBoxLayout *valueBox = new QHBoxLayout();
    valueBox->addWidget(labelRegister);
    valueBox->addWidget(lineEdit);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(valueBox);

    addButton("I - " + tr("Input"));
    addButton("O - " + tr("Output"));
    addButton("G - " + tr("Gate"));
    addButton("N - " + tr("Normalization"));
    addButton("R - " + tr("RGB-LED"));
    addButton("X - " + tr("Special"));
}

void InputOutputSelector::addButton(QString label)
{
    QPushButton *button = new QPushButton(label);
    buttons.append(button);
    mainLayout->addWidget(button);

}
