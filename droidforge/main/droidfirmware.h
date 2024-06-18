#ifndef DROIDFIRMWARE_H
#define DROIDFIRMWARE_H

#include "circuit.h"
#include <QJsonDocument>
#include <QJsonObject>


class DroidFirmware
{
    QJsonDocument json;
    QJsonObject circuits;
    QJsonObject controllers;
    QJsonObject pagerefs;

public:
    DroidFirmware();
    QString version() const;
    unsigned availableMemory(unsigned master) const;
    unsigned initialJacktableSize() const;
    unsigned hasManualPage(QString pageref) const;
    unsigned manualPage(QString pageref) const;
    bool circuitExists(QString circuit) const;
    unsigned circuitManualPage(QString circuit) const;
    unsigned circuitBaseRAM(QString circuit) const;
    bool circuitIsPersisted(QString circuit) const;
    bool circuitIsDeprecated(QString circuit) const;
    QString jackRamHint(QString circuit, QString jack) const;
    unsigned controllerUsedRAM(QString circuit) const;
    QString canonizeJackName(QString circuit, QString jack) const;
    bool jackIsInput(QString circuit, QString jack) const;
    bool jackIsOutput(QString circuit, QString jack) const;
    unsigned jackArraySize(QString circuit, QString prefix, bool isInput) const;
    QString jackShortname(QString circuit, QString jack) const;
    QString jackTypeSymbol(QString circuit, QString prefix, bool isInput) const;
    QStringList circuitsOfCategory(QString category) const;
    QString circuitDescription(QString circuit) const;
    QString circuitTitle(QString circuit) const;
    QStringList inputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL) const;
    QStringList outputsOfCircuit(QString circuit, jackselection_t sel = JACKSELECTION_FULL) const;
    QStringList jacksOfCircuit(QString circuit, QString whence, jackselection_t) const;
    QStringList jackGroupsOfCircuit(QString circuit, QString whence, QString search) const;
    QString jackDescriptionHTML(QString circuit, QString whence, QString jack) const;
    QString jackTypeDescriptionHTML(QString circuit, QString whence, QString jack) const;
    QString jackTypeSymbol(QString circuit, QString whence, QString jack) const;
    QMap<double, QString> jackValueTable(QString circuit, QString whence, QString jack) const;
    bool jackHasDefaultvalue(QString circuit, QString jack) const;
    double jackDefaultvalue(QString circuit, QString jack) const;
    unsigned numControllerRegisters(const QString &controller, char registerType) const;
    unsigned numGlobalRegisters(char registerType) const;
    bool circuitNeedsMIDI(QString circuit) const;
    bool circuitNeedsMaster18(QString circuit) const;
    bool checkAllDescriptions() const;

private:
    QJsonValue findJack(QString circuit, QString whence, QString jack) const;
    QJsonValue findJackArray(QString circuit, QString whence, QString prefix) const;
    QString delatexify(QString s, bool html=false) const;
    void replaceLatexSymbols(QString &s) const;
    QString jackTableAsString(const QMap<double, QString> &table) const;
    QString jackTypeExplanation(QString symbol, bool isInput) const;
    bool isDigits(QString s) const;
};

extern DroidFirmware *the_firmware;

#endif // DROIDFIRMWARE_H
