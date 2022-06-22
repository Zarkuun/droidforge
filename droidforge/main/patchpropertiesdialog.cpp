#include "patchpropertiesdialog.h"
#include "inputvalidationexception.h"
#include "mainwindow.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QGridLayout>

PatchPropertiesDialog::PatchPropertiesDialog(QWidget *parent)
    : Dialog("patchproperties", parent)
{
    QLabel *labelTitle = new QLabel(tr("Title:"));
    lineEditTitle = new QLineEdit(this);

    QLabel *labelDescription = new QLabel(tr("Description:"));
    textEditDescription = new QTextEdit(this);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    layout->addWidget(labelTitle, 0, 0);
    layout->addWidget(lineEditTitle, 0, 1);
    layout->addWidget(labelDescription, 1, 0);
    layout->addWidget(textEditDescription, 1, 1);
    layout->addWidget(buttonBox, 2, 0, 1, 2);
}
bool PatchPropertiesDialog::editPatchProperties(PatchEditEngine *patch)
{
    static PatchPropertiesDialog *dialog = 0;
    if (!dialog)
        dialog = new PatchPropertiesDialog();

    QString oldTitle = patch->getTitle();
    dialog->lineEditTitle->setText(oldTitle);

    QString oldDescription = patch->getDescription();
    dialog->textEditDescription->setText(oldDescription);

    while (true) {
        if (dialog->exec() == QDialog::Accepted) {
            try {
                dialog->validateInput();
                QString newTitle = dialog->lineEditTitle->text().trimmed();
                QString newDescription = dialog->textEditDescription->toPlainText();
                if (newTitle != oldTitle ||
                        newDescription != oldDescription)
                {
                    patch->setTitle(newTitle);
                    patch->setDescription(newDescription);
                    return true;
                }
                else
                    return false;
            }
            catch (InputValidationException &e) {
                QMessageBox box(
                            QMessageBox::Critical,
                            tr("Cannot save patch properties"),
                            e.toString(),
                            QMessageBox::Ok,
                            dialog);
                box.exec();
                e.getWidget()->setFocus();
            }
        }
        else
            return false;
    }
}
bool PatchPropertiesDialog::validateInput()
{
     if (lineEditTitle->text().trimmed().isEmpty()) {
         throw InputValidationException(
                     lineEditTitle,
                     tr("Please specify a title for your patch"));
         return false;
     }
     else
         return true;
}
