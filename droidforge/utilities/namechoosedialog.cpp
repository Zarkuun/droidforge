#include "namechoosedialog.h"
#include "tuning.h"
#include "globals.h"

#include <QGridLayout>
#include <QDialogButtonBox>

NameChooseDialog::NameChooseDialog(QWidget *parent)
    : Dialog{"namechooser", parent}
    , forceUpper(false)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    // Label
    label = new QLabel();
    layout->addWidget(label, 0, 0);

    // Text editor
    lineEdit = new QLineEdit();
    layout->addWidget(lineEdit, 0, 1);
    connect(lineEdit, &QLineEdit::textChanged, this, &NameChooseDialog::changeText);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox, 1, 1, 1, 2);
}

QString NameChooseDialog::getReName(const QString &title, const QString &label, QString oldname, bool forceUpperCase)
{
    return getName(title, label, oldname, forceUpperCase, true);
}
QString NameChooseDialog::getNewName(const QString &title, const QString &label, QString oldname, bool forceUpperCase)
{
    return getName(title, label, oldname, forceUpperCase, false);
}
QString NameChooseDialog::getName(const QString &title, const QString &label, QString oldname, bool forceUpperCase, bool rename)
{
    static NameChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new NameChooseDialog();

    dialog->setForceUpperCase(forceUpperCase);
    dialog->setWindowTitle(title);
    dialog->label->setText(label);
    dialog->lineEdit->setText(oldname);
    dialog->lineEdit->selectAll();

    if (dialog->exec() == QDialog::Accepted)
        return dialog->lineEdit->text().trimmed();
    else {
        if (rename)
            return oldname;
        else
            return "";
    }
}

void NameChooseDialog::changeText(const QString &)
{
   if (forceUpper) {
       int curPos = lineEdit->cursorPosition();
       lineEdit->setText(lineEdit->text().toUpper());
       lineEdit->setCursorPosition(curPos);
   }
}
