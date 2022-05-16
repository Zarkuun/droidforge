#include "controllerchoosedialog.h"
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QKeyEvent>

ControllerChooseDialog::ControllerChooseDialog(QWidget *parent)
    : Dialog("controllerchooser", parent)
{
    setWindowTitle(tr("Add controller"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // Area with clickable controllers
    controllerSelector = new ControllerSelector(this);
    connect(controllerSelector, &ControllerSelector::controllerSelected, this, &QDialog::accept);
    mainLayout->addWidget(controllerSelector);

    // OK, Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

QString ControllerChooseDialog::chooseController()
{
    static ControllerChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new ControllerChooseDialog(the_forge);

    dialog->grabKeyboard();
    int result = dialog->exec();
    dialog->releaseKeyboard();

    if (result == QDialog::Accepted)
        return dialog->getSelectedController();
    else
        return "";
}

const QString &ControllerChooseDialog::getSelectedController() const
{
    return controllerSelector->getSelectedController();
}

void ControllerChooseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
        controllerSelector->moveCursor(-1);
    else if (event->key() == Qt::Key_Right)
       controllerSelector->moveCursor(1);
    else
        Dialog::keyPressEvent(event);
}
