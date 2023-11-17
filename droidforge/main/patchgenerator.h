#ifndef PATCHGENERATOR_H
#define PATCHGENERATOR_H

#include <QString>
#include <QJsonDocument>

#include "patch.h"

class PatchGenerator
{
    bool _valid;
    QString _name; // just the file name
    QString _path;
    QString _interpreter;
    QString _error;
    QString _title;
    QJsonDocument _parameterInfo;

public:
    PatchGenerator(QString path, QString name);
    bool isValid() const { return _valid; };
    QString run(const QStringList &args, bool &ok);
    QString title() const { return _title; };
    QString name() const { return _name; };
    QString error() const { return _error; }
    const QJsonDocument &parameterInfo() const { return _parameterInfo; };
};

#endif // PATCHGENERATOR_H
