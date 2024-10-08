#include "jackvaluetabledialog.h"
#include "droidfirmware.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

JackValueTableDialog::JackValueTableDialog(QString circuit, QString jack, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Select predefined value for parameter %1").arg(jack));
    auto jackValueTable = the_firmware->jackValueTable(circuit, "inputs", jack);

    QGridLayout *mainLayout = new QGridLayout();
    setLayout(mainLayout);

    int row = 0;
    for (auto value: jackValueTable.keys())
    {
        QString description = jackValueTable[value];
        QPushButton *button = new QPushButton(QString::number(value), this);
        connect(button, &QPushButton::pressed, this, [this,value] () { valueSelected(value); });
        QLabel *label = new QLabel(description, this);
        mainLayout->addWidget(button, row, 0);
        mainLayout->addWidget(label, row, 1);
        row ++;
    }
}

void JackValueTableDialog::valueSelected(double value)
{
    selectedValue = value;
    accept();
}
