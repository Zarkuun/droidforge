#include "controllerlabellingdialog.h"

#include <QGridLayout>
#include <QDialogButtonBox>

ControllerLabellingDialog::ControllerLabellingDialog(QWidget *parent)
    : Dialog("controllerlabelling", parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox, 2, 0, 1, 2);
}
