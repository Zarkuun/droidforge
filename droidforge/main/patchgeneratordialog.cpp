#include "patchgeneratordialog.h"
#include "parseexception.h"
#include "patch.h"
#include "patchparser.h"
#include "usermanual.h"
#include "hintdialog.h"
#include "globals.h"

#include <QRandomGenerator>
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QPushButton>

PatchGeneratorDialog::PatchGeneratorDialog(PatchGenerator *generator, QWidget *parent)
    : Dialog(generator->title(), parent)
    , _generator(generator)
{
    setWindowTitle(tr("Generate patch") + " - " + _generator->title());
    setDefaultSize({500, 600});

    setStyleSheet("QCheckBox { margin-right: 3px; } "
                  "QLineEdit { margin-right: 5px; } ");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *bottomBox = new QHBoxLayout();
    setLayout(mainLayout);

    renderOptions(mainLayout);
    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QPushButton *presetButton = new QPushButton(tr("Load preset:"));
    connect(presetButton, &QPushButton::clicked, this, &PatchGeneratorDialog::loadPreset);
    bottomBox->addWidget(presetButton);
    _presetChoice = createPresetChoice();
    bottomBox->addWidget(_presetChoice);

    QString hinttext =
      tr("Welcome to the patch generators!\n\n"
         "A patch generator creates a complete and read-to-use DROID patch for\n"
         "you, based on your decisions in the dialog that I will show you now.\n"
         "You can load one of several example presets into the dialog wit the\n"
         "button \"Load preset\".\n\n"
         "Once you are done, press \"OK\". This will generate the patch. If you\n"
         "like, you can rearrange the CV outputs and buttons via drag & drop.\n"
         "Then load the patch as usually to your master with \"Activate!\" or \n"
         "\"Save to SD!\".");

    if (_generator->description() != "") {
        QPushButton *infoButton = new QPushButton(tr("Info"));
        connect(infoButton, &QPushButton::clicked, this, &PatchGeneratorDialog::showInfo);
        buttonBox->addButton(infoButton, QDialogButtonBox::ActionRole);
    }

    if (the_manual->hasTopic("pg-" + _generator->name())) {
        QPushButton *manualButton = new QPushButton(tr("Manual"));
        connect(manualButton, &QPushButton::clicked, this, &PatchGeneratorDialog::manual);
        buttonBox->addButton(manualButton, QDialogButtonBox::ActionRole);
        hinttext +=
          tr("\n\nDon't forget the button \"Manual\"! It shows the comprehensive\n"
             "user manual of this patch generator.");
    }

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    bottomBox->addWidget(buttonBox);
    mainLayout->addLayout(bottomBox);

    HintDialog::hint("patch_generator", hinttext);
}

void PatchGeneratorDialog::renderOptions(QLayout *mainLayout)
{
    QTabWidget *tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    const QJsonDocument &info = _generator->parameterInfo();

    auto sections = info.object()["sections"].toArray();
    for (auto s: sections)
    {
        auto section = s.toObject();
        QWidget *box = new QWidget();
        tabWidget->addTab(box, section["title"].toString());
        auto boxLayout = new QVBoxLayout;
        box->setLayout(boxLayout);

        if (section.contains("help")) {
            QString helptext = section["help"].toString();
            QLabel *label = new QLabel(helptext);
            label->setWordWrap(true);
            boxLayout->addWidget(label);
            boxLayout->addSpacing(20);
        }

        auto options = section["options"].toArray();
        for (auto o: options) {
            auto option = o.toObject();
            QString optionTitle = option["title"].toString();
            QString optionName  = option["name"].toString();
            QHBoxLayout *hori = new QHBoxLayout();
            hori->setContentsMargins(0, 0, 0, 0);
            boxLayout->addLayout(hori);
            QLabel *label = new QLabel(optionTitle);
            hori->addWidget(label);
            hori->addStretch(1);

            // Enumerations
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

            // Numbers
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

            // Boolean (True/False)
            else { // checkbox
                QCheckBox *cb = new QCheckBox(box);
                _booleanFields[optionName] = cb;
                hori->addWidget(cb);

                // QComboBox *cb = new QComboBox(box);
                // _booleanFields[optionName] = cb;
                // hori->addWidget(cb);
                // cb->addItem(tr("yes"));
                // cb->addItem(tr("no"));
            }

            label->setText(label->text() + ":");

            // This looks like shit
            // if (option.contains("help")) {
            //     QString helpText = option["help"].toString();
            //     QLabel *label = new QLabel(helpText);
            //     boxLayout->addWidget(label);
            // }
        }
        boxLayout->addStretch();
    }

    int i = lastOpenTab();
    if (i < tabWidget->count())
        tabWidget->setCurrentIndex(i);

    connect(tabWidget, &QTabWidget::currentChanged, this, &PatchGeneratorDialog::tabChanged);
}
void PatchGeneratorDialog::loadPreset()
{
    QString preset = _presetChoice->currentData().toString();
    pgconfig_t config;
    configForPreset(preset, config);
    setConfig(config);
}
void PatchGeneratorDialog::manual()
{
    the_manual->showTopic("pg-" + _generator->name());
}
void PatchGeneratorDialog::showInfo()
{
    QMessageBox::information(
        this,
        _generator->title(),
        _generator->description());
}
void PatchGeneratorDialog::tabChanged(int index)
{
    QSettings settings;
    settings.setValue("pg-current-tab-" + _generator->name(), index);
}
int PatchGeneratorDialog::lastOpenTab()
{
    QSettings settings;
    return settings.value("pg-current-tab-" + _generator->name(), 0).toInt();
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
        auto *cb = _booleanFields[name];
        // config[name] = cb->currentIndex() == 0 ? true : false;
        config[name] = cb->isChecked();
    }
}
void PatchGeneratorDialog::defaultConfig(pgconfig_t &config)
{
    const QJsonDocument &info = _generator->parameterInfo();
    auto presets = info["presets"].toArray();
    QString first;
    for (auto p: presets) {
        auto preset = p.toObject();
        QString name = preset["name"].toString();
        if (name == "default") {
            configForPreset(name, config);
            return;
        }
        else if (first == "")
            first = name;
    }
    configForPreset(first, config);
}
void PatchGeneratorDialog::configForPreset(QString presetName, pgconfig_t &config)
{
    const QJsonDocument &info = _generator->parameterInfo();
    auto presets = info["presets"].toArray();
    for (auto p: presets) {
        auto preset = p.toObject();
        QString name = preset["name"].toString();
        if (name != presetName)
            continue;
        auto parameters = preset["parameters"].toObject();
        for (auto &key: parameters.keys()) {
            auto value = parameters[key].toVariant();
            config[key] = value;
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
        if (settings.contains(vpath)) {
            config[key] = settings.value(vpath);
        }
    }
}
QComboBox *PatchGeneratorDialog::createPresetChoice()
{
    QComboBox *box = new QComboBox();
    const QJsonDocument &info = _generator->parameterInfo();
    auto presets = info.object()["presets"].toArray();
    for (auto p: presets) {
        auto preset = p.toObject();
        QString name = preset["name"].toString();
        QString title = preset["title"].toString();
        box->addItem(title, name);
    }
    return box;
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
        auto *cb = _booleanFields[name];
        bool checked = value.toBool();
        cb->setChecked(checked);
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
