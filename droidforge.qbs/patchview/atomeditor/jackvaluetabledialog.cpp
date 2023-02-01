#include "jackvaluetabledialog.h"
#include "droidfirmware.h"
#include "globals.h"

#include <QGridLayout>
#include <QLabel>

JackValueTableDialog::JackValueTableDialog(QString circuit, QString jack, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Select predefined value for parameter %1").arg(jack));
    auto jackValueTable = the_firmware->jackValueTable(circuit, "inputs", jack);

    QGridLayout *mainLayout = new QGridLayout();
    setLayout(mainLayout);

    int row = 0;
    for (auto it = jackValueTable.keyBegin();
         it != jackValueTable.keyEnd();
         ++it)
    {
        float value = *it;
        QString description = jackValueTable[value];
        QPushButton *button = new QPushButton(QString::number(value), this);
        connect(button, &QPushButton::pressed, this, [this,value] () { valueSelected(value); });
        QLabel *label = new QLabel(description, this);
        mainLayout->addWidget(button, row, 0);
        mainLayout->addWidget(label, row, 1);
        row ++;
    }
}

void JackValueTableDialog::valueSelected(float value)
{
    selectedValue = value;
    accept();
}
