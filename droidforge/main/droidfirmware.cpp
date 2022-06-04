#include "droidfirmware.h"
#include "circuitchoosedialog.h"
#include "globals.h"
#include "registertypes.h"

#include <QFile>
#include <QJsonArray>

DroidFirmware *the_firmware = 0;

DroidFirmware::DroidFirmware()
{
    the_firmware = this;

    QFile file;
    file.setFileName(":droidfirmware.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open firmware information file.");
    }
    else {
        QByteArray content = file.readAll();
        json = QJsonDocument::fromJson(content);
        file.close();
        circuits = json["circuits"].toObject();
    }
}


bool DroidFirmware::circuitExists(QString circuit) const
{
    return circuits.keys().contains(circuit);
}

QString DroidFirmware::canonizeJackName(QString circuit, QString jack) const
{
    // Some circuits have array like pitch1...pitch16. In the DROID patch,
    // however, the user may write "pitch" as a shorthand for pitch1. In
    // Order to avoid problems we always use the full names in the DROID forge.
    if (jackIsInput(circuit, jack) || jackIsOutput(circuit, jack))
        return jack;

    QString withone = jack + "1";
    if (jackIsInput(circuit, withone) || jackIsOutput(circuit, withone))
        return withone;

    return jack;
}


bool DroidFirmware::jackIsInput(QString circuit, QString jack) const
{
    // TODO: Wenn ein jack als input und output vorkommt,
    // aber in einem als array, dann klappt die Erkennung
    // hier nicht gut genug!
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    return !jackinfo.isNull();
}


bool DroidFirmware::jackIsOutput(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "outputs", jack);
    return !jackinfo.isNull();
}


unsigned DroidFirmware::jackArraySize(QString circuit, QString jack, bool isInput) const
{
    QJsonValue jackinfo;

    if (isInput)
        jackinfo = findJackArray(circuit, "inputs", jack);
    else
        jackinfo = findJackArray(circuit, "outputs", jack);
    if (jackinfo.isNull())
        return 0;

    QJsonObject ji = jackinfo.toObject();
    if (ji.contains("count"))
        return jackinfo["count"].toInt(1);
    else
        return 0;
}


QStringList DroidFirmware::circuitsOfCategory(QString category) const
{
    QStringList result;
    for (auto i=circuits.constBegin(); i != circuits.constEnd(); ++i)
    {
        QString name = i.key();
        QString cat = i.value().toObject()["category"].toString();
        if (category.isEmpty() || cat == category)
            result.append(name);
    }
    return result;
}

QString DroidFirmware::circuitDescription(QString circuit) const
{
    QString fullDescription = circuits[circuit].toObject()["description"].toString();
    return fullDescription.split('.')[0].replace("\n", " ");
}

QStringList DroidFirmware::inputsOfCircuit(QString circuit, jackselection_t jackSelection) const
{
    return jacksOfCircuit(circuit, "inputs", jackSelection);
}

QStringList DroidFirmware::outputsOfCircuit(QString circuit, jackselection_t jackSelection) const
{
    return jacksOfCircuit(circuit, "outputs", jackSelection);
}


QStringList DroidFirmware::jacksOfCircuit(QString circuit, QString whence, jackselection_t jackSelection) const
{
    QStringList result;
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();

    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        int essential = jackinfo["essential"].toInt(0);
        // essential is 0, 1 or 2 (0 = none, 1 = typical, 2 = essential)
        // jackSelection is 0 -> all, ... 3 -> none
        if (essential >= jackSelection) {
            if (jackinfo.contains("count")) {
                int count = jackinfo["count"].toInt(1);
                for (int i=1; i<=count; i++) {
                    QString name = jackinfo["prefix"].toString() + QString::number(i);
                    result.append(name);
                }
            }
            else
                result.append(jackinfo["name"].toString());
        }
    }
    return result;
}

QStringList DroidFirmware::jackGroupsOfCircuit(QString circuit, QString whence, QString search) const
{
    QStringList result;
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        QString name;
        if (jackinfo.contains("count"))
            name = jackinfo["prefix"].toString();
        else
            name = jackinfo["name"].toString();
        if (name.contains(search, Qt::CaseInsensitive))
            result.append(name);
    }
    return result;
}

unsigned DroidFirmware::numGlobalRegisters(char registerType) const
{
    switch (registerType) {
    case REGISTER_INPUT:     return 8;
    case REGISTER_NORMALIZE: return 8;
    case REGISTER_OUTPUT:    return 8;
    case REGISTER_GATE:      return 12;
    case REGISTER_RGB_LED:   return 32;
    case REGISTER_EXTRA:     return 1;
    default:                 return 0;
    }

}

unsigned DroidFirmware::numControllerRegisters(const QString &module, char registerType) const
{
    if (module == "p4b2") {
        switch (registerType) {
        case REGISTER_POT:    return 4;
        case REGISTER_BUTTON: return 2;
        case REGISTER_LED:    return 2;
        }
    }
    else if (module == "p2b8") {
        switch (registerType) {
        case REGISTER_POT:    return 2;
        case REGISTER_BUTTON: return 8;
        case REGISTER_LED:    return 8;
        }
    }
    else if (module == "p10") {
        switch (registerType) {
        case REGISTER_POT:    return 10;
        }
    }
    else if (module == "b32") {
        switch (registerType) {
        case REGISTER_BUTTON: return 32;
        case REGISTER_LED:    return 32;
        }
    }
    else if (module == "s10") {
        switch (registerType) {
        case REGISTER_SWITCH: return 10;
        }
    }
    else if (module == "m4") {
        switch (registerType) {
        case REGISTER_POT:     return 4;
        case REGISTER_BUTTON:  return 4;
        case REGISTER_RGB_LED: return 4;
        case REGISTER_LED:     return 4;
        }
    }

    return 0;
}


QJsonValue DroidFirmware::findJack(QString circuit, QString whence, QString jack) const
{
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        // Account for jack arrays
        if (jackinfo.contains("count")) {
            for (qsizetype i=1; i<=jackinfo["count"].toInt(); i++) {
                QString n = jackinfo["prefix"].toString() + QString::number(i);
                if (n == jack)
                    return jackinfo;
            }
            continue;
        }
        else if (jackinfo["name"] == jack)
            return jackinfo;
    }
    return QJsonValue(QJsonValue::Null);
}

QJsonValue DroidFirmware::findJackArray(QString circuit, QString whence, QString prefix) const
{
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        if (jackinfo["prefix"].toString() == prefix) {
            return jackinfo;
        }
    }
    return 0;
}
