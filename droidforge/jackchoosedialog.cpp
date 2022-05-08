#include "jackchoosedialog.h"
#include "jackselector.h"
#include "droidfirmware.h"

#include <QGridLayout>

JackChooseDialog::JackChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    resize(900, 600);
    setWindowTitle(tr("Add new jack / parameter"));

    // Canvas with circuit diagram
    jackSelector = new JackSelector(this);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(buttonBox, 1, 0);
    mainLayout->addWidget(jackSelector, 0, 0);
    setLayout(mainLayout);
}

JackChooseDialog::~JackChooseDialog()
{

}

void JackChooseDialog::setCircuit(const QString &circuit)
{
    jackSelector->setCircuit(circuit);
}

QString JackChooseDialog::getSelectedJack() const
{
    return jackSelector->getSelectedJack();
}
