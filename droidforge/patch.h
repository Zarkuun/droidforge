#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"

class Patch
{
public:
    QString title;
    QString description;
    QString libraryId;
    unsigned version;
    QList<QString> controllers;
    QList<PatchSection> sections;
};

#endif // PATCH_H
