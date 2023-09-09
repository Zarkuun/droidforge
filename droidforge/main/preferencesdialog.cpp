#include "preferencesdialog.h"
#include "globals.h"
#include "tuning.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : Dialog("Preferences", parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QGroupBox *box;
    QVBoxLayout *layout;

    // Patch compression
    box = new QGroupBox(tr("Compress patch before loading into master"));
    layout = new QVBoxLayout(box);
    checkboxRenameCables = new QCheckBox(tr("Rename patch cables to _A, _B, etc. (makes patch less readable)"));
    checkboxRemoveEmptyLines = new QCheckBox(tr("Remove empty lines (mixes up line numbers in error messages)"));
    layout->addWidget(checkboxRemoveEmptyLines);
    layout->addWidget(checkboxRenameCables);
    mainLayout->addWidget(box);

    // Patch validation
    box = new QGroupBox(tr("Patch validation"));
    layout = new QVBoxLayout(box);
    checkboxIgnoreUnknownJacks = new QCheckBox(tr("Do not treat unknown parameters as errors"));
    layout->addWidget(checkboxIgnoreUnknownJacks);
    mainLayout->addWidget(box);

    // Polling for activation
    box = new QGroupBox(tr("Activation / Loading of patches"));
    layout = new QVBoxLayout(box);
    checkboxPollX7 = new QCheckBox(tr("Poll regularily for X7 connection"));
    checkboxPollSD = new QCheckBox(tr("Poll regularily for DROID SD card"));
    layout->addWidget(checkboxPollX7);
    layout->addWidget(checkboxPollSD);
    mainLayout->addWidget(box);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}
void PreferencesDialog::loadSettings()
{
    QSettings settings;
    checkboxRenameCables->setChecked(settings.value("compression/rename_cables", false).toBool());
    checkboxRemoveEmptyLines->setChecked(settings.value("compression/remove_empty_lines", false).toBool());
    checkboxIgnoreUnknownJacks->setChecked(settings.value("validation/ignore_unknown_jacks", false).toBool());
    checkboxPollX7->setChecked(settings.value("activation/poll_for_x7", SETTING_POLL_DEFAULT).toBool());
    checkboxPollSD->setChecked(settings.value("activation/poll_for_sd", SETTING_POLL_DEFAULT).toBool());

}
void PreferencesDialog::saveSettings() const
{
    QSettings settings;
    settings.setValue("compression/rename_cables", checkboxRenameCables->isChecked());
    settings.setValue("compression/remove_empty_lines", checkboxRemoveEmptyLines->isChecked());
    settings.setValue("validation/ignore_unknown_jacks", checkboxIgnoreUnknownJacks->isChecked());
    settings.setValue("activation/poll_for_x7", checkboxPollX7->isChecked());
    settings.setValue("activation/poll_for_sd", checkboxPollSD->isChecked());
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
