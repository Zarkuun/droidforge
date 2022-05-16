#include "controllerchoosedialog.h"

#include <QVBoxLayout>

ControllerChooseDialog::ControllerChooseDialog(QWidget *parent)
    : Dialog("controllerchooser", parent)
{
    setWindowTitle(tr("Add controller"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(buttonBox);
}

QString ControllerChooseDialog::chooseController()
{
    static ControllerChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new ControllerChooseDialog();

    if (dialog->exec() == QDialog::Accepted)
        return "b32" ; // dialog->getSelectedJack();
    else
        return "";
}
