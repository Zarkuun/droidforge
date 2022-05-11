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
        AWAITING_TITLE_COMMENT,
        DESCRIPTION,
        HEAD,
        SECTION_HEADER_ACTIVE,
        CIRCUIT_HEADER,
    } commentState;
    // bool nextCommentIsSectionTitle;

public:
    PatchParser();
    void parse(QString fileName, Patch *patch);

private:
    void parseLine(QString line);
    void parseEmptyLine();
    void parseCommentLine(QString line);
    void parseLibraryMetaData(QString data);
    void parseCircuitLine(QString line);
    void parseJackLine(Circuit *circuit, QString line);
    void parseCircuit(QString name);
    void stripEmptyCommentLines();
    void startNewSection(QString name);
    bool maybeParseMetaComment(QString comment);
    bool maybeParseRegisterComment(QString comment);
};

#endif // PATCHPARSER_H
