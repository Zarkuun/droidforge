#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "dialog.h"

#include <QObject>
#include <QCheckBox>
#include <QLineEdit>

class PreferencesDialog : public Dialog
{
    Q_OBJECT

    QCheckBox *checkboxRenameCables;
    QCheckBox *checkboxUseShortnames;
    QCheckBox *checkboxDeduplicateJacks;
    QCheckBox *checkboxIgnoreUnknownJacks;
    QCheckBox *checkboxDenounceDeprecatedCircuits;
    QCheckBox *checkboxPollX7;
    QCheckBox *checkboxPollSD;
    QCheckBox *checkboxShipFirmware;
    QLineEdit *lineEditPythonExecutable;

public:
    static void editPreferences();

private:
    PreferencesDialog(QWidget *parent = nullptr);
    bool validate() const;
    void loadSettings();
    void saveSettings() const;
};

#endif // PREFERENCESDIALOG_H
