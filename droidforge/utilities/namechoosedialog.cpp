#include "namechoosedialog.h"
#include "tuning.h"

#include <QGridLayout>
#include <QDialogButtonBox>

NameChooseDialog::NameChooseDialog(QWidget *parent)
    : Dialog{"namechooser", parent}
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    // Label
    label = new QLabel();
    layout->addWidget(label, 0, 0);

    // Text editor
    lineEdit = new QLineEdit();
    layout->addWidget(lineEdit, 0, 1);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox, 1, 1, 1, 2);
}

QString NameChooseDialog::getName(const QString &title, const QString &label, QString oldname)
{
     static NameChooseDialog *dialog = 0;
     if (!dialog)
         dialog = new NameChooseDialog();

     dialog->setWindowTitle(title);
     dialog->label->setText(label);
     dialog->lineEdit->setText(oldname);
     dialog->lineEdit->selectAll();
    if (dialog->exec() == QDialog::Accepted)
        return dialog->lineEdit->text().trimmed();
    else
        return oldname;
}
