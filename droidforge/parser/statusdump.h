#ifndef STATUSDUMP_H
#define STATUSDUMP_H

#include <QString>
#include <QMap>

class StatusDump
{
    QString filePath;
    QMap<QString, double> cables;
    QMap<QString, double> registers;
    QMap<QString, QString> cableCompressionMapping; // compensate compression

public:
    StatusDump(const QString &filePath);
    void addCableMapping(const QMap<QString, QString> mapping);
    const QString &title() const { return filePath; };
    bool hasCable(QString name) const;
    bool hasRegister(const QString &name) const;
    double valueOfCable(QString name) const;
    double valueOfRegister(const QString &name) const;

private:
    void parse(const QStringList &lines);
    void parseCableLine(const QString &line);
    void parseRegisterLine(const QString &line);
};

#endif // STATUSDUMP_H
