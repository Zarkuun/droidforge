#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "dialog.h"

#include <QObject>
#include <QCheckBox>

class PreferencesDialog : public Dialog
{
    Q_OBJECT

    QCheckBox *checkboxRemoveEmptyLines;
    QCheckBox *checkboxRenameCables;
    QCheckBox *checkboxIgnoreUnknownJacks;
    QCheckBox *checkboxPollX7;
    QCheckBox *checkboxPollSD;

public:
    static void editPreferences();

private:
    PreferencesDialog(QWidget *parent = nullptr);
    void loadSettings();
    void saveSettings() const;
};

#endif // PREFERENCESDIALOG_H
