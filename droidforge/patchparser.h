#ifndef PATCHPARSER_H
#define PATCHPARSER_H

#include "patch.h"

#include <QFile>



class PatchParser
{
    Patch *patch; // used while parsing
    PatchSection *section;
    Circuit *circuit;

    QString errorMessage;
    unsigned errorLine;
    QStringList currentComment;
    QString sectionHeader;
    enum {
        TITLE,
        DESCRIPTION,
        HEAD,
        SECTION_HEADER_ACTIVE,
        CIRCUIT_HEADER,
    } commentState;
    bool nextCommentIsSectionTitle;

public:
    PatchParser();
    bool parse(QString fileName, Patch *patch);

private:
    bool parseLine(QString line);
    bool parseEmptyLine();
    bool parseCommentLine(QString line);
    bool parseCircuitLine(QString line);
    bool parseJackLine(QString line);
    bool parseController(QString name);
    bool parseCircuit(QString name);
    void stripEmptyCommentLines();
    void startNewSection(QString name);
};

#endif // PATCHPARSER_H
