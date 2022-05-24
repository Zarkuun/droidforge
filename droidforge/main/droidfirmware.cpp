#include "droidfirmware.h"
#include "circuitchoosedialog.h"

#include <QFile>
#include <QJsonArray>

DroidFirmware::DroidFirmware()
{
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


bool DroidFirmware::circuitExists(QString circuit)
{
    return circuits.contains(circuit);
}


bool DroidFirmware::jackIsInput(QString circuit, QString jack)
{
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    return !jackinfo.isNull();
}


bool DroidFirmware::jackIsOutput(QString circuit, QString jack)
{
    QJsonValue jackinfo = findJack(circuit, "outputs", jack);
    return !jackinfo.isNull();
}


unsigned DroidFirmware::jackArraySize(QString circuit, QString jack)
{
    QJsonValue jackinfo = findJackArray(circuit, "inputs", jack);
    if (jackinfo.isNull())
        jackinfo = findJackArray(circuit, "outputs", jack);
    if (jackinfo.isNull())
        return 0;

    QJsonObject ji = jackinfo.toObject();
    if (ji.contains("count"))
        return jackinfo["count"].toInt(1);
    else
        return 0;
}


QStringList DroidFirmware::circuitsOfCategory(QString category)
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

QString DroidFirmware::circuitDescription(QString circuit)
{
    QString fullDescription = circuits[circuit].toObject()["description"].toString();
    return fullDescription.split('.')[0].replace("\n", " ");
}

QStringList DroidFirmware::inputsOfCircuit(QString circuit, jackselection_t jackSelection)
{
    return jacksOfCircuit(circuit, "inputs", jackSelection);
}

QStringList DroidFirmware::outputsOfCircuit(QString circuit, jackselection_t jackSelection)
{
    return jacksOfCircuit(circuit, "outputs", jackSelection);
}


QStringList DroidFirmware::jacksOfCircuit(QString circuit, QString whence, jackselection_t jackSelection)
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


QStringList DroidFirmware::jackGroupsOfCircuit(QString circuit, QString whence, QString search)
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


QJsonValue DroidFirmware::findJack(QString circuit, QString whence, QString jack)
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

QJsonValue DroidFirmware::findJackArray(QString circuit, QString whence, QString prefix)
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
