#include "hintdialog.h"

#include <QGridLayout>
#include <QSettings>

HintDialog::HintDialog(QWidget *parent)
    : Dialog("hint", parent)
{
    // Content: the message
    label = new QLabel("Here comes the hint");

    // Checkbox
    checkbox = new QCheckBox(tr("Show this hint again next time"));

    // OK-Button
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    // Layout
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    layout->addWidget(label, 0, 0, 1, 2);
    layout->addWidget(checkbox, 1, 0);
    layout->addWidget(buttonBox, 1, 1);
}

void HintDialog::hint(const QString &id, const QString &content)
{
    static HintDialog *dialog = 0;
    if (!dialog)
        dialog = new HintDialog();

    QSettings settings;
    QString key = "hint/" + id;
    bool showHint = settings.contains(key) ? settings.value(key).toBool() : true;
    if (!showHint) {
        return; // Hint already shown
    }

    dialog->checkbox->setChecked(true);
    dialog->label->setText(content);
    dialog->exec();
    settings.setValue(key, dialog->checkbox->isChecked());
}
