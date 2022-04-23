#include "patchpropertiesdialog.h"
#include "ui_patchpropertiesdialog.h"

#include <QKeyEvent>

PatchPropertiesDialog::PatchPropertiesDialog(const QString &title, const QString &description, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PatchPropertiesDialog)
{
    ui->setupUi(this);
    ui->lineEditTitle->setText(title);
    ui->textEditDescription->setText(description);
}


PatchPropertiesDialog::~PatchPropertiesDialog()
{
    delete ui;
}

QString PatchPropertiesDialog::getTitle()
{
    return ui->lineEditTitle->text();
}

QString PatchPropertiesDialog::getDescription()
{
    return ui->textEditDescription->toPlainText();
}
