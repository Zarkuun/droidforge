#include "patchsection.h"

PatchSection::~PatchSection()
{
    for (qsizetype i=0; i<circuits.length(); i++)
        delete circuits[i];

}

PatchSection *PatchSection::clone() const
{
    PatchSection *newsection = new PatchSection(title);
    for (unsigned i=0; i<circuits.size(); i++)
        newsection->circuits.append(circuits[i]->clone());
    return newsection;
}


QString PatchSection::toString()
{
    QString s;

    if (!title.isEmpty()) {
        s += "-------------------------------------------------\n";
        s += title + "\n";
        s += "-------------------------------------------------\n\n";
    }

    for (qsizetype i=0; i<circuits.length(); i++)
        s += circuits[i]->toString();

    return s;
}


void PatchSection::deleteCircuitNr(unsigned nr)
{
    Circuit *c = circuits[nr];
    circuits.remove(nr);
    delete c;
}
