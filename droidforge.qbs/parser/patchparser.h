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
    Patch *parseFile(QString filePath);
    void parseFile(QString filePath, Patch *patch);
    void parseString(QString source, Patch *patch);

private:
    void parse(QStringList lines, Patch *patch);
    void parseLine(QString line);
    void parseEmptyLine();
    void parseCommentLine(QString line);
    void parseLibraryMetaData(QString data);
    void parseCircuitLine(QString line, bool disabled);
    void parseJackLine(Circuit *circuit, QString line, bool disabled);
    void parseCircuit(QString name, bool disabled);
    void stripEmptyCommentLines();
    void startNewSection(QString name);
    bool maybeParseMetaComment(QString comment);
    bool maybeParseRegisterComment(QString comment);
};

#endif // PATCHPARSER_H
