#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"

#include <QList>

class PatchSection
{
public:
    QString title;
    QList<Circuit> circuits;
};

#endif // PATCHSECTION_H
