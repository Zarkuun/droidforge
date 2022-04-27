#include "droidfirmware.h"

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


QStringList DroidFirmware::circuitsOfCategory(QString category)
{
    QStringList result;
    for (auto i=circuits.constBegin(); i != circuits.constEnd(); ++i)
    {
        QString name = i.key();
        QString cat = i.value().toObject()["category"].toString();
        if (cat == category)
            result.append(name);
    }
    return result;
}

QString DroidFirmware::circuitDescription(QString circuit)
{
    QString fullDescription = circuits[circuit].toObject()["description"].toString();
    return fullDescription.split('.')[0];
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
