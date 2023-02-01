#include "patchpropertiesdialog.h"
#include "inputvalidationexception.h"
#include "mainwindow.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>

PatchPropertiesDialog::PatchPropertiesDialog(QWidget *parent)
    : Dialog("patchproperties", parent)
{
    lineEditTitle = new QLineEdit(this);
    textEditDescription = new QTextEdit(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->addRow(tr("Title"), lineEditTitle);
    formLayout->addRow(tr("Description"), textEditDescription);
    mainLayout->addLayout(formLayout);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
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
