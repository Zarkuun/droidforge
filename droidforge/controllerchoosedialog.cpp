#include "controllerchoosedialog.h"
#include "mainwindow.h"
#include "rackview.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDesktopServices>

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
    QPushButton *purchaseButton = new QPushButton(tr("Purchase"));
    connect(purchaseButton, &QPushButton::clicked, this, &ControllerChooseDialog::purchase);
    buttonBox->addButton(purchaseButton, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setFocusPolicy(Qt::StrongFocus);
}

QString ControllerChooseDialog::chooseController()
{
    static ControllerChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new ControllerChooseDialog();

    dialog->setFocus();
    if (dialog->exec() == QDialog::Accepted)
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
    else if (event->key() == Qt::Key_Home)
       controllerSelector->moveCursorHome();
    else if (event->key() == Qt::Key_End)
       controllerSelector->moveCursorEnd();
    else
        Dialog::keyPressEvent(event);
}

void ControllerChooseDialog::purchase()
{
    QString name = controllerSelector->getSelectedController();
    if (name != "")
        QDesktopServices::openUrl(QUrl(SHOP_PRODUCTS_URL + name));
}
