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
    QString version() const;
    bool circuitExists(QString circuit) const;
    unsigned circuitManualPage(QString circuit) const;
    QString canonizeJackName(QString circuit, QString jack) const;
    bool jackIsInput(QString circuit, QString jack) const;
    bool jackIsOutput(QString circuit, QString jack) const;
    bool jackIsArray(QString circuit, QString jack) const;
    unsigned jackArraySize(QString circuit, QString prefix, bool isInput) const;
    QStringList circuitsOfCategory(QString category) const;
    QString circuitDescription(QString circuit) const;
    QString circuitTitle(QString circuit) const;
    QStringList inputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL) const;
    QStringList outputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL) const;
    QStringList jacksOfCircuit(QString circuit, QString whence, jackselection_t) const;
    QStringList jackGroupsOfCircuit(QString circuit, QString whence, QString search) const;
    QString jackDescriptionHTML(QString circuit, QString whence, QString jack) const;
    QMap<float, QString> jackValueTable(QString circuit, QString whence, QString jack) const;
    bool jackHasDefaultvalue(QString circuit, QString whence, QString jack) const;
    float jackDefaultvalue(QString circuit, QString whence, QString jack) const;
    unsigned numControllerRegisters(const QString &controller, char registerType) const;
    unsigned numGlobalRegisters(char registerType) const;

private:
    QJsonValue findJack(QString circuit, QString whence, QString jack) const;
    QJsonValue findJackArray(QString circuit, QString whence, QString prefix) const;
    QString delatexify(QString s, bool html=false) const;
    void replaceLatexSymbols(QString &s) const;
};

extern DroidFirmware *the_firmware;

#endif // DROIDFIRMWARE_H
