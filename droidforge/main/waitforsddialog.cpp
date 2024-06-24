#include "waitforsddialog.h"

WaitForSDDialog::WaitForSDDialog(QString title, QString text, MainWindow *mainWindow, PatchOperator *po)
    : Dialog("waitforsd", mainWindow)
    , patchOperator(po)
{
    setWindowTitle(title);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QLabel *label = new QLabel(text);
    mainLayout->addWidget(label);

    // OK, Cancel
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setFocusPolicy(Qt::StrongFocus);
}

QString WaitForSDDialog::waitForSD()
{
    while (true) {
        QString dir = patchOperator->sdCardDir();
        if (dir != "")
            return dir;
        if (exec() == QDialog::Rejected)
            return "";
    }
}

void WaitForSDDialog::showEvent(QShowEvent *)
{
    QTimer::singleShot(500, this, &WaitForSDDialog::timedCheckSD);
}

void WaitForSDDialog::timedCheckSD()
{
    if (patchOperator->sdCardDir() != "")
        accept();
    else
        QTimer::singleShot(500, this, &WaitForSDDialog::timedCheckSD);
}
