#include "preferencesdialog.h"
#include "droidfirmware.h"
#include "tuning.h"
#include "hintdialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSettings>
#include <QFile>
#include <QMessageBox>

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
    checkboxUseShortnames = new QCheckBox(tr("Use abbreviated parameter names like b instead of button"));
    checkboxDeduplicateJacks = new QCheckBox(tr("Detect and share duplicate values for inputs"));
    layout->addWidget(checkboxRenameCables);
    layout->addWidget(checkboxUseShortnames);
    layout->addWidget(checkboxDeduplicateJacks);
    mainLayout->addWidget(box);

    // Patch validation
    box = new QGroupBox(tr("Patch validation"));
    layout = new QVBoxLayout(box);
    checkboxIgnoreUnknownJacks = new QCheckBox(tr("Do not treat unknown parameters as errors"));
    layout->addWidget(checkboxIgnoreUnknownJacks);
    checkboxDenounceDeprecatedCircuits = new QCheckBox(tr("Denounce deprecated circuits"));
    layout->addWidget(checkboxDenounceDeprecatedCircuits);
    mainLayout->addWidget(box);

    // Polling for activation
    box = new QGroupBox(tr("Activation (Loading of patches)"));
    layout = new QVBoxLayout(box);
    checkboxPollX7 = new QCheckBox(tr("Poll regularly for X7 connection"));
    checkboxPollSD = new QCheckBox(tr("Poll regularly for DROID SD card"));
    checkboxShipFirmware = new QCheckBox(tr("Always save master's firmware to SD card (firmware upgrade)"));
    layout->addWidget(checkboxPollX7);
    layout->addWidget(checkboxPollSD);
    layout->addWidget(checkboxShipFirmware);
    mainLayout->addWidget(box);

    // Patch generators
    box = new QGroupBox(tr("Patch generators"));
    auto *hlayout = new QHBoxLayout(box);
    QLabel *lab = new QLabel(tr("Path to Python3 executable"));
    lineEditPythonExecutable = new QLineEdit();
    hlayout->addWidget(lab);
    hlayout->addWidget(lineEditPythonExecutable);
    mainLayout->addWidget(box);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}
bool PreferencesDialog::validate() const
{
    QString pp = lineEditPythonExecutable->text().trimmed();
    if (pp == "")
        return true; // no path set

    QFile file(pp);
    if (!file.exists()) {
        QMessageBox::critical(
            0,
            tr("File not found"),
            tr("Invalid path of the Python interpreter. This file does not exist."));
        return false;
    }
    return true;
}
void PreferencesDialog::loadSettings()
{
    QSettings settings;
    checkboxRenameCables->setChecked(settings.value("compression/rename_cables", false).toBool());
    checkboxUseShortnames->setChecked(settings.value("compression/use_shortnames", false).toBool());
    checkboxDeduplicateJacks->setChecked(settings.value("compression/deduplicate_jacks", false).toBool());
    checkboxIgnoreUnknownJacks->setChecked(settings.value("validation/ignore_unknown_jacks", false).toBool());
    checkboxDenounceDeprecatedCircuits->setChecked(settings.value("validation/denounce_deprecated_circuits", true).toBool());
    checkboxPollX7->setChecked(settings.value("activation/poll_for_x7", SETTING_POLL_DEFAULT).toBool());
    checkboxPollSD->setChecked(settings.value("activation/poll_for_sd", SETTING_POLL_DEFAULT).toBool());
    checkboxShipFirmware->setChecked(settings.value("activation/ship_firmware", false).toBool());
    lineEditPythonExecutable->setText(settings.value("system/python_path", "").toString());
}
void PreferencesDialog::saveSettings() const
{
    QSettings settings;
    settings.setValue("compression/rename_cables", checkboxRenameCables->isChecked());
    settings.setValue("compression/use_shortnames", checkboxUseShortnames->isChecked());
    settings.setValue("compression/deduplicate_jacks", checkboxDeduplicateJacks->isChecked());
    settings.setValue("validation/ignore_unknown_jacks", checkboxIgnoreUnknownJacks->isChecked());
    settings.setValue("validation/denounce_deprecated_circuits", checkboxDenounceDeprecatedCircuits->isChecked());
    settings.setValue("activation/poll_for_x7", checkboxPollX7->isChecked());
    settings.setValue("activation/poll_for_sd", checkboxPollSD->isChecked());
    settings.setValue("activation/ship_firmware", checkboxShipFirmware->isChecked());
    settings.setValue("system/python_path", lineEditPythonExecutable->text().trimmed());
}
void PreferencesDialog::editPreferences()
{
    static PreferencesDialog *dialog = 0;
    if (!dialog)
        dialog = new PreferencesDialog();

    dialog->loadSettings();
    while (true) {
        if (dialog->exec() == QDialog::Accepted)
        {
            if (!dialog->validate())
                continue;

            dialog->saveSettings();
            QSettings settings;
            if (settings.value("compression/deduplicate_jacks", false).toBool())
            {
                HintDialog::hint("jack_deduplication",
                                 tr("You have enabled sharing of common values of inputs and outputs.\n\n"
                                    "For this to work just must make sure that your master module has\n"
                                    "the firmware version %1. Otherwise the patch might not load\n"
                                    "or even crash in an endless loop.\n\n"
                                    "In case of trouble disable this setting and do a factory reset\n"
                                    "on your master.")
                                     .arg(the_firmware->version()));
            }
        }
        break;
    }
}
