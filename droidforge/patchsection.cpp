#include "patchsection.h"

PatchSection::~PatchSection()
{
    for (qsizetype i=0; i<circuits.length(); i++)
        delete circuits[i];

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
