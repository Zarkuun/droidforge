#ifndef PATCHPARSER_H
#define PATCHPARSER_H

#include "patch.h"

#include <QFile>

class PatchParser
{
    Patch *patch; // used while parsing
    QString errorMessage;
    unsigned errorLine;

public:
    PatchParser();
    bool parse(QString fileName, Patch *patch);

private:
    void parseLine(QString line);
};

#endif // PATCHPARSER_H
