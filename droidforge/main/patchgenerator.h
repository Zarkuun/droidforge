#ifndef PATCHGENERATOR_H
#define PATCHGENERATOR_H

#include <QString>
#include <QJsonDocument>

#include "patch.h"

class PatchGenerator
{
    bool _valid;
    QString _path;
    QString _interpreter;
    QString _error;
    QString _title;
    QJsonDocument _parameterInfo;

public:
    PatchGenerator(QString path);
    bool isValid() const { return _valid; };
    QString run(const QStringList &args, bool &ok);
    QString title() const { return _title; };
    QString error() const { return _error; }
    const QJsonDocument &parameterInfo() const { return _parameterInfo; };
};

#endif // PATCHGENERATOR_H
