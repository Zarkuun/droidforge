#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"

#include <QStringList>

class Patch
{
    QStringList controllers;

public:
    Patch();
    ~Patch();
    Patch *clone() const;
    qsizetype numControllers() const { return controllers.size(); };
    QString controller(qsizetype i) const { return controllers[i]; };
    void addController(QString name) { controllers.append(name); };
    bool saveToFile(QString filename);

    QString title;
    QStringList description;
    QString libraryId;
    unsigned version;
    QList<PatchSection *> sections;

    QString toString();
};

#endif // PATCH_H
