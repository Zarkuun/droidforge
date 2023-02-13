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
    checkboxRenameCables = new QCheckBox(tr("Rename patch cables to _A, _B, etc. (makes patch less readable)"));
    checkboxRemoveEmptyLines = new QCheckBox(tr("Remove empty lines (mixes up line numbers in error messages)"));
    QVBoxLayout *compressBoxLayout = new QVBoxLayout(compressBox);
    compressBoxLayout->addWidget(checkboxRemoveEmptyLines);
    compressBoxLayout->addWidget(checkboxRenameCables);

    QGroupBox *loadingOfPatches = new QGroupBox(tr("Loading of patches"));
    checkboxIgnoreUnknownJacks = new QCheckBox(tr("Do not treat unknown parameters as errors"));
    QVBoxLayout *loadingOfPatchesLayout = new QVBoxLayout(loadingOfPatches);
    loadingOfPatchesLayout->addWidget(checkboxIgnoreUnknownJacks);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    mainLayout->addWidget(compressBox);
    mainLayout->addWidget(loadingOfPatches);
    mainLayout->addWidget(buttonBox);
}
void PreferencesDialog::loadSettings()
{
    QSettings settings;
    checkboxRenameCables->setChecked(settings.value("compression/rename_cables", false).toBool());
    checkboxRemoveEmptyLines->setChecked(settings.value("compression/remove_empty_lines", false).toBool());
    checkboxIgnoreUnknownJacks->setChecked(settings.value("loading/ignore_unknown_jacks", false).toBool());

}
void PreferencesDialog::saveSettings() const
{
    QSettings settings;
    settings.setValue("compression/rename_cables", checkboxRenameCables->isChecked());
    settings.setValue("compression/remove_empty_lines", checkboxRemoveEmptyLines->isChecked());
    settings.setValue("loading/ignore_unknown_jacks", checkboxIgnoreUnknownJacks->isChecked());
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
