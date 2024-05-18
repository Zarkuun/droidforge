#include "controllerchoosedialog.h"
#include "mainwindow.h"
#include "rackview.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QPushButton>

ControllerChooseDialog::ControllerChooseDialog(MainWindow *mainWindow)
    : Dialog("controllerchooser", mainWindow)
    , mainWindow(mainWindow)
{
    setWindowTitle(tr("Add controller"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // Area with clickable controllers
    controllerSelector = new ControllerSelector(mainWindow, this);
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
    setFocus();
    if (exec() == QDialog::Accepted)
        return getSelectedController();
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
