#ifndef STATUSDUMP_H
#define STATUSDUMP_H

#include <QString>
#include <QMap>

class StatusDump
{
    QMap<QString, double> cables;
    QMap<QString, double> registers;

public:
    StatusDump(const QString &filePath);
    bool hasCable(const QString &name) const;
    bool hasRegister(const QString &name) const;
    double valueOfCable(const QString &name) const;
    double valueOfRegister(const QString &name) const;

private:
    void parse(const QStringList &lines);
    void parseCableLine(const QString &line);
    void parseRegisterLine(const QString &line);
};

#endif // STATUSDUMP_H
