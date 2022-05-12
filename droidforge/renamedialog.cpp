#include "renamedialog.h"
#include "tuning.h"

#include <QGridLayout>

RenameDialog::RenameDialog(QWidget *parent)
    : QDialog{parent}
{
    resize(REDI_WIDTH, REDI_HEIGHT);
    setWindowTitle(tr("Rename"));

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    // Label
    label = new QLabel("FOO");
    layout->addWidget(label, 0, 0);

    // Text editor
    lineEdit = new QLineEdit();
    layout->addWidget(lineEdit, 0, 1);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox, 1, 1, 1, 2);
}

QString RenameDialog::getRenameName(const QString &title, const QString &label, const QString &oldname)
{
     static RenameDialog *dialog = 0;
     if (!dialog)
         dialog = new RenameDialog();

     dialog->setWindowTitle(title);
     dialog->label->setText(label);
     dialog->lineEdit->setText(oldname);
     dialog->lineEdit->selectAll();
    if (dialog->exec() == QDialog::Accepted)
        return dialog->lineEdit->text().trimmed();
    else
        return oldname;
}
