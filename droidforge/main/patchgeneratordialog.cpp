#include "patchgeneratordialog.h"
#include "globals.h"
#include "parseexception.h"
#include "patch.h"
#include "patchparser.h"

#include <QSettings>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

PatchGeneratorDialog::PatchGeneratorDialog(PatchGenerator *generator, QWidget *parent)
    : Dialog(generator->title(), parent)
    , _generator(generator)
{
    setWindowTitle(tr("Generate patch") + " - " + _generator->title());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    renderOptions(mainLayout);
    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *defaultsButton = new QPushButton(tr("Reset to defaults"));
    connect(defaultsButton, &QPushButton::clicked, this, &PatchGeneratorDialog::resetToDefaults);
    buttonBox->addButton(defaultsButton, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);


    // TODO: Hier muss man jetzt die ganze Optionen anzeigen.
    // Jede Sektion in einer GroupBox. Und wir müssen die Funktionalität
    // machen, dass man Werte von Defaults laden kann. Außerdem braucht
    // jeder PG noch in den Settings die letzen Einstellungen. Die
    // muss man auch Laden können. Irgendwie als map oder so. Mit QT-Datentyp.

    // Wenn man dann OK drückt, rufen wir den Generator auf mit diesen
    // OPtionen. Wenn er einen Fehler meldet, zeigen wir diesen an.
    // Wenn nicht, erzeugen wir ein Patch und geben das zurück.

    // Im PatchOperator muss man dann das aktuell geladene Patch durch
    // das neu generiert ersetzen. Falls es modified ist, müssen wir
    // fragen, etc.

    // Dann müssen wir auch noch den Workflow schnell machen. Also
    // dass man schnell eine neue Version machen kann.

    // Frage ist, ob der neue Patch als "modified" erscheint.
}

void PatchGeneratorDialog::renderOptions(QLayout *layout)
{
    const QJsonDocument &info = _generator->parameterInfo();
    auto sections = info.object()["sections"].toArray();
    for (auto s: sections)
    {
        auto section = s.toObject();
        auto box = new QGroupBox(section["title"].toString());
        auto boxLayout = new QVBoxLayout;
        box->setLayout(boxLayout);
        layout->addWidget(box);
        // boxLayout->setContentsMargins(0, 0, 0, 0);

        auto options = section["options"].toArray();
        for (auto o: options) {
            auto option = o.toObject();
            QString optionTitle = option["title"].toString();
            QString optionName  = option["name"].toString();
            QHBoxLayout *hori = new QHBoxLayout(box);
            hori->setContentsMargins(0, 0, 0, 0);
            boxLayout->addLayout(hori);
            QLabel *label = new QLabel(optionTitle);
            hori->addWidget(label);
            hori->addStretch(1);

            if (option.contains("enum")) {
                QComboBox *cb = new QComboBox(box);
                _enumFields[optionName] = cb;
                hori->addWidget(cb);
                auto entries = option["enum"].toArray();
                for (auto e: entries) {
                    auto pair = e.toArray();
                    cb->addItem(pair[1].toString(), pair[0].toString());
                }
            }
            else if (option.contains("number")) {
                QLineEdit *le = new QLineEdit(box);
                _numberFields[optionName] = le;
                int minimum = option["number"].toArray()[0].toInt();
                int maximum = option["number"].toArray()[1].toInt();
                le->setMaxLength(4);
                le->setFixedWidth(50);
                hori->addWidget(le);
                label->setText(label->text() + " (" + QString::number(minimum) + " - " + QString::number(maximum) + ")");
            }
            else { // checkbox
                QComboBox *cb = new QComboBox(box);
                _booleanFields[optionName] = cb;
                hori->addWidget(cb);
                cb->addItem(tr("yes"));
                cb->addItem(tr("no"));
            }

            label->setText(label->text() + ":");
        }
    }
}
void PatchGeneratorDialog::resetToDefaults()
{
    pgconfig_t options;
    defaultConfig(options);
    setConfig(options);
}
void PatchGeneratorDialog::collectConfig(pgconfig_t &config)
{
    for (auto it = _numberFields.constKeyValueBegin();  it != _numberFields.constKeyValueEnd(); ++it) {
        QString name = it->first;
        QLineEdit *le = _numberFields[name];
        config[name] = le->text().toInt();
    }

    for (auto it = _enumFields.constKeyValueBegin();  it != _enumFields.constKeyValueEnd(); ++it) {
        QString name = it->first;
        QComboBox *cb = _enumFields[name];
        config[name] = cb->currentData();
    }

    for (auto it = _booleanFields.constKeyValueBegin();  it != _booleanFields.constKeyValueEnd(); ++it) {
        QString name = it->first;
        QComboBox *cb = _booleanFields[name];
        config[name] = cb->currentIndex() == 0 ? true : false;
    }
}
void PatchGeneratorDialog::defaultConfig(pgconfig_t &config)
{
    const QJsonDocument &info = _generator->parameterInfo();
    auto sections = info.object()["sections"].toArray();
    for (auto s: sections)
    {
        auto section = s.toObject();
        auto options = section["options"].toArray();
        for (auto o: options) {
            auto option = o.toObject();
            auto def = option["default"];
            config[option["name"].toString()] = def.toVariant();
        }
    }
}
void PatchGeneratorDialog::setConfig(pgconfig_t &config)
{
    for (auto it = config.constKeyValueBegin();  it != config.constKeyValueEnd(); ++it)
    {
        QString key = it->first;
        setOption(key, config[key]);
    }
}
void PatchGeneratorDialog::saveConfigToSettings(pgconfig_t &config)
{
    QSettings settings;
    QString path = "patch_generators/" + _generator->name();
    for (auto it = config.constKeyValueBegin();  it != config.constKeyValueEnd(); ++it)
    {
        QString key = it->first;
        QVariant value = it->second;
        settings.setValue(path + "/" + key, value);
    }
}
void PatchGeneratorDialog::loadConfigFromSettings(pgconfig_t &config)
{
    QSettings settings;
    QString path = "patch_generators/" + _generator->name();
    defaultConfig(config);
    for (auto it = config.constKeyValueBegin();  it != config.constKeyValueEnd(); ++it)
    {
        QString key = it->first;
        QString vpath = path + "/" + key;
        if (settings.contains(vpath))
            config[key] = settings.value(vpath);
    }
}
void PatchGeneratorDialog::setOption(QString name, QVariant value)
{
    if (_enumFields.contains(name)) {
        QString key = value.toString();
        QComboBox *cb = _enumFields[name];
        for (int i=0; i<cb->count(); i++) {
            if (cb->itemData(i) == key) {
                cb->setCurrentIndex(i);
                break;
            }
        }
    }
    else if (_numberFields.contains(name)) {
        QLineEdit *le = _numberFields[name];
        int num = value.toInt();
        le->setText(QString::number(num));
    }
    else if (_booleanFields.contains(name)) {
        QComboBox *cb = _booleanFields[name];
        bool checked = value.toBool();
        cb->setCurrentIndex(checked ? 0 : 1);
    }
}
Patch *PatchGeneratorDialog::generatePatch(PatchGenerator *generator)
{
    PatchGeneratorDialog dialog(generator);
    pgconfig_t startConfig;
    dialog.loadConfigFromSettings(startConfig);
    dialog.setConfig(startConfig);

    while (true) {
        if (dialog.exec() == QDialog::Accepted) {
            pgconfig_t config;
            dialog.collectConfig(config);
            QStringList args;
            for (auto it = config.constKeyValueBegin();  it != config.constKeyValueEnd(); ++it)
            {
                QString key = it->first;
                QString svalue;
                QVariant value = config[key];
                if (!strcmp(value.typeName(), "bool"))
                    svalue = value.toBool() ? "1" : "0";
                else
                    svalue = value.toString();
                args.append(key + "=" + svalue);
            }
            bool ok;
            QString patchSource = generator->run(args, ok);
            if (ok) {
                dialog.saveConfigToSettings(config);
                try {
                    PatchParser parser;
                    Patch parsed;
                    parser.parseString(patchSource, &parsed);
                    return parsed.clone();
                }
                catch (ParseException &e) {
                    QMessageBox::critical(
                                &dialog,
                                tr("Parse error"),
                                e.toString(),
                                QMessageBox::Ok);
                }
            }
            else {
                QMessageBox::critical(
                            &dialog,
                            tr("Failed to generate patch"),
                            patchSource,
                            QMessageBox::Ok);
            }
            continue;
        }
        else
            return 0;
    }
}
