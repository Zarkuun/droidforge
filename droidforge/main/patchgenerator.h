#ifndef PATCHGENERATOR_H
#define PATCHGENERATOR_H


#include <QString>
#include <QJsonDocument>

class PatchGenerator
{
    bool _valid;
    QString _path;
    QString _interpreter;
    QString _error;
    QJsonDocument _parameterInfo;
    QString _title;

public:
    PatchGenerator(QString path);
    bool isValid() const { return _valid; };
    QString run(const QStringList &args, bool &ok);
    QString title() const { return _title; };
    QString error() const { return _error; }
};

#endif // PATCHGENERATOR_H
