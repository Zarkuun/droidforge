#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"

#include <QStringList>

class Patch
{

public:
    Patch();

    QString title;
    QStringList description;
    QString libraryId;
    unsigned version;
    QStringList controllers;
    QList<PatchSection> sections;

    QString toString();
};

#endif // PATCH_H
