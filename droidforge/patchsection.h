#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"

#include <QList>

class PatchSection
{
public:
    PatchSection(QString t) : title(t) {};
    ~PatchSection();

    QString title;
    QList<Circuit *> circuits;
    QString toString();

    void deleteCircuitNr(unsigned nr);
};

#endif // PATCHSECTION_H
