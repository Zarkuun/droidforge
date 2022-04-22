#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"

#include <QStringList>

class Patch
{
    QString title;
    QStringList controllers;

public:
    Patch();
    ~Patch();
    Patch *clone() const;
    qsizetype numControllers() const { return controllers.size(); };
    QString controller(qsizetype i) const { return controllers[i]; };
    void addController(QString name) { controllers.append(name); };
    bool saveToFile(QString filename);

    QStringList description;
    QString libraryId;
    unsigned version;
    QList<PatchSection *> sections;

    QString toString();
    const QString &getTitle() const;
    void setTitle(const QString &newTitle);
};

#endif // PATCH_H
