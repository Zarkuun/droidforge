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
    bool circuitExists(QString circuit) const;
    bool jackIsInput(QString circuit, QString jack) const;
    bool jackIsOutput(QString circuit, QString jack) const;
    unsigned jackArraySize(QString circuit, QString jack) const;
    QStringList circuitsOfCategory(QString category) const;
    QString circuitDescription(QString circuit) const;
    QStringList inputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL) const;
    QStringList outputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL) const;
    QStringList jacksOfCircuit(QString circuit, QString whence, jackselection_t) const;
    QStringList jackGroupsOfCircuit(QString circuit, QString whence, QString search) const;
    unsigned numControllerRegisters(const QString &controller, char registerType) const;
    unsigned numGlobalRegisters(char registerType) const;

private:
    QJsonValue findJack(QString circuit, QString whence, QString jack) const;
    QJsonValue findJackArray(QString circuit, QString whence, QString prefix) const;
};

extern DroidFirmware *the_firmware;

#endif // DROIDFIRMWARE_H
