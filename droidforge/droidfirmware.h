#ifndef DROIDFIRMWARE_H
#define DROIDFIRMWARE_H

#include "circuitchoosedialog.h"

#include <QJsonDocument>
#include <QJsonObject>


class DroidFirmware
{
    QJsonDocument json;
    QJsonObject circuits;

public:
    DroidFirmware();
    bool circuitExists(QString circuit);
    bool jackIsInput(QString circuit, QString jack);
    bool jackIsOutput(QString circuit, QString jack);
    unsigned jackArraySize(QString circuit, QString jack);
    QStringList circuitsOfCategory(QString category);
    QString circuitDescription(QString circuit);
    QStringList inputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL);
    QStringList outputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL);
    QStringList jacksOfCircuit(QString circuit, QString whence, jackselection_t);
    QStringList jackGroupsOfCircuit(QString circuit, QString whence, QString search);

private:
    QJsonValue findJack(QString circuit, QString whence, QString jack);
    QJsonValue findJackArray(QString circuit, QString whence, QString prefix);
};

extern DroidFirmware *the_firmware;

#endif // DROIDFIRMWARE_H
