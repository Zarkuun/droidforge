#ifndef PATCHGENERATORDIALOG_H
#define PATCHGENERATORDIALOG_H

#include "patch.h"
#include "patchgenerator.h"
#include "dialog.h"

#include <QMap>

typedef QMap<QString, QVariant> pgconfig_t;

class PatchGeneratorDialog : public Dialog
{
    Q_OBJECT

    PatchGenerator *_generator;
    QMap <QString, QLineEdit *> _numberFields;
    QMap <QString, QComboBox *> _enumFields;
    QMap <QString, QCheckBox *> _booleanFields;
    QComboBox *_presetChoice;

public:
    PatchGeneratorDialog(PatchGenerator *generator, QWidget *parent = nullptr);
    static Patch *generatePatch(PatchGenerator *generator);

private:
    void renderOptions(QLayout *mainLayout);
    void setOption(QString name, QVariant value);
    void collectConfig(pgconfig_t &config);
    void defaultConfig(pgconfig_t &config);
    void configForPreset(QString preset, pgconfig_t &config);
    void setConfig(pgconfig_t &config);
    void saveConfigToSettings(pgconfig_t &config);
    void loadConfigFromSettings(pgconfig_t &config);
    QComboBox *createPresetChoice();

private slots:
    void loadPreset();
    void randomize();
};

#endif // PATCHGENERATORDIALOG_H
