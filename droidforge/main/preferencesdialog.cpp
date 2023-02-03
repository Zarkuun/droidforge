#include "preferencesdialog.h"
#include "globals.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : Dialog("Preferences", parent)
{
    QGroupBox *compressBox = new QGroupBox(tr("Compress patch before loading into master"));
    checkboxRemoveSpaces = new QCheckBox(tr("Remove spaces"));
    checkboxRenameCables = new QCheckBox(tr("Rename patch cables to _A, _B, etc. (makes patch less readable)"));
    checkboxRemoveEmptyLines = new QCheckBox(tr("Remove empty lines (mixes up line numbers in error messages)"));
    QVBoxLayout *compressBoxLayout = new QVBoxLayout(compressBox);
    compressBoxLayout->addWidget(checkboxRemoveSpaces);
    compressBoxLayout->addWidget(checkboxRemoveEmptyLines);
    compressBoxLayout->addWidget(checkboxRenameCables);
    compressBox->setLayout(compressBoxLayout);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    mainLayout->addWidget(compressBox);
    mainLayout->addWidget(buttonBox);
}
void PreferencesDialog::loadSettings()
{
    QSettings settings;
    checkboxRemoveSpaces->setChecked(settings.value("compression/remove_spaces", true).toBool());
    checkboxRenameCables->setChecked(settings.value("compression/rename_cables", false).toBool());
    checkboxRemoveEmptyLines->setChecked(settings.value("compression/remove_empty_lines", false).toBool());
}
void PreferencesDialog::saveSettings() const
{
    QSettings settings;
    settings.setValue("compression/remove_spaces", checkboxRemoveSpaces->isChecked());
    settings.setValue("compression/rename_cables", checkboxRenameCables->isChecked());
    settings.setValue("compression/remove_empty_lines", checkboxRemoveEmptyLines->isChecked());
}
void PreferencesDialog::editPreferences()
{
    static PreferencesDialog *dialog = 0;
    if (!dialog)
        dialog = new PreferencesDialog();

    dialog->loadSettings();
    if (dialog->exec() == QDialog::Accepted)
        dialog->saveSettings();
}
