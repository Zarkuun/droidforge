#include "patchpropertiesdialog.h"
#include "inputvalidationexception.h"
#include "ui_patchpropertiesdialog.h"
#include "mainwindow.h"

#include <QKeyEvent>
#include <QMessageBox>

PatchPropertiesDialog::PatchPropertiesDialog(Patch *patch, QWidget *parent)
    : QDialog(parent)
    , patch(patch)
    , ui(new Ui::PatchPropertiesDialog)
{
    ui->setupUi(this);
    ui->lineEditTitle->setText(patch->getTitle());
    ui->textEditDescription->setText(patch->getDescription());
    connect(this, &PatchPropertiesDialog::accepted, this, &PatchPropertiesDialog::apply);
}


PatchPropertiesDialog::~PatchPropertiesDialog()
{
    delete ui;
}


void PatchPropertiesDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        try {
            validateInput();
        }
        catch (InputValidationException &e) {
            QMessageBox box(
                        QMessageBox::Critical,
                        tr("Cannot save patch properties"),
                        e.toString(),
                        QMessageBox::Ok,
                        this);
            box.exec();
            e.getWidget()->setFocus();
            return;
        }
    }
    QDialog::done(r);
}


bool PatchPropertiesDialog::validateInput()
{
     if (ui->lineEditTitle->text().trimmed().isEmpty()) {
         throw InputValidationException(
                      ui->lineEditTitle,
                     tr("Please specify a title for your patch"));
         return false;
     }
     else
         return true;
}


void PatchPropertiesDialog::apply()
{
    the_forge->registerEdit(tr("Editing patch properties"));
    patch->setTitle(ui->lineEditTitle->text().trimmed());
    patch->setDescription(ui->textEditDescription->toPlainText());
}
