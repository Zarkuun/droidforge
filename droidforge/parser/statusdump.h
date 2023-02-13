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

private:
    void parse(const QStringList &lines);
    void parseCableLine(const QString &line);
    void parseRegisterLine(const QString &line);
};

#endif // STATUSDUMP_H
