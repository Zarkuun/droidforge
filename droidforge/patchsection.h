#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"

#include <QList>

class PatchSection
{
public:
    PatchSection(QString t) : title(t) {};
    ~PatchSection();
    PatchSection *clone() const;
    QString toString();
    void deleteCircuitNr(unsigned nr);

    QString title;
    QList<Circuit *> circuits;

};

#endif // PATCHSECTION_H
