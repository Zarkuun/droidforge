#include "jackchoosedialog.h"
#include "droidfirmware.h"

#include <QGridLayout>

JackChooseDialog::JackChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    resize(300, 100);
    setWindowTitle(tr("Add new jack / parameter"));

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(buttonBox, 1, 3);
    setLayout(mainLayout);
}

JackChooseDialog::~JackChooseDialog()
{

}

void JackChooseDialog::setCircuit(QString name)
{


}



