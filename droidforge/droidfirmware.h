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
    QStringList circuitsOfCategory(QString category);
    QString circuitDescription(QString circuit);
    QStringList essentialInputs(QString circuit, jackselection_t);
    QStringList essentialOutputs(QString circuit, jackselection_t);

private:
    QJsonValue findJack(QString circuit, QString whence, QString jack);
    QStringList essentialJacks(QString circuit, QString whence, jackselection_t);
};

extern DroidFirmware *the_firmware;

#endif // DROIDFIRMWARE_H
