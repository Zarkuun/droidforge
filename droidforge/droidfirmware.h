#ifndef DROIDFIRMWARE_H
#define DROIDFIRMWARE_H

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

private:
    QJsonValue findJack(QString circuit, QString whence, QString jack);
};

extern DroidFirmware *the_firmware;

#endif // DROIDFIRMWARE_H
