#include "globals.h"
#include "mainwindow.h"
#include "patchparser.h"
#include "modulebuilder.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "parseexception.h"
#include "registerlabels.h"

#include <QTextStream>
#include <QRegularExpression>
#include <QException>


PatchParser::PatchParser()
    : patch(0)
    , section(0)
    , circuit(0)
{
}
Patch *PatchParser::parseFile(QString filePath)
{
    Patch *patch = new Patch;
    try {
        parseFile(filePath, patch);
    }
    catch (ParseException &e) {
        delete patch;
        return 0;
    }
    return patch;
}
void PatchParser::parseFile(QString filePath, Patch *patch)
{
    QStringList lines;
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        throw ParseException("Cannot open file: " + inputFile.errorString());
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    inputFile.close();
    parse(lines, patch);
}
void PatchParser::parseString(QString source, Patch *patch)
{
    QStringList lines = source.split('\n');
    parse(lines, patch);
}
void PatchParser::parse(QStringList lines, Patch *patch)
{
    commentState = AWAITING_TITLE_COMMENT;
    errorMessage = "";
    errorLine = 0;
    section = 0;
    circuit = 0;
    this->patch = patch;

    QString lineerrors;
    for (auto line: lines) {
        errorLine ++;
        try {
            parseLine(line);
        }
        catch (ParseException &e) {
            QString error = QString("Line ") + QString::number(errorLine) + ": " + e.toString() + "\n";
            lineerrors += error;
        }
    }
    if (!lineerrors.isEmpty())
        throw ParseException("Syntax errors in your patch:\n\n" + lineerrors);

    if (patch->numSections() == 0) {
        patch->addSection(new PatchSection());
    }
}
void PatchParser::parseLine(QString line)
{
    line = line.trimmed();
    if (line.isEmpty())
        parseEmptyLine();
    else if (line[0] == '#')
        parseCommentLine(line);
    else if (line[0] == '[')
        parseCircuitLine(line, false /* not disabled */);
    else if (line[0].isLetter() && line.contains('=')) {
        if (!circuit)
            throw ParseException("Jack assignment before any circuit was declared.");
        else
            parseJackLine(circuit, line, false /* not disabled */);
    }
    else
        throw ParseException("Garbled line");
}
void PatchParser::parseEmptyLine()
{
    if (commentState == AWAITING_TITLE_COMMENT) // no title
        commentState = DESCRIPTION;

    else if (commentState == DESCRIPTION) {
        patch->addDescriptionLines(currentComment);
        currentComment.clear();
    }

    else if (currentComment.size() > 0) {
        currentComment.append("");
    }
}
void PatchParser::parseCommentLine(QString line)
{
    static QRegularExpression disabledJackLine("^[a-zA-z]+([1-9][0-9]*)?[[:space:]]*=.*");

    QString comment = line.mid(1).trimmed(); // strip off '#'

    // Is this a disabled circuit?
    if (comment.startsWith("[")) {
        parseCircuitLine(comment, true /* disabled */);
        return;
    }

    // Or maybe a disabled jack line?
    QRegularExpressionMatch m = disabledJackLine.match(comment);
    if (m.hasMatch()) {
        parseJackLine(circuit, comment, true /* disabled */);
        return;
    }

    // Comments like "# ------------" start (or end) section headers
    if (comment.startsWith("---")) {
        if (commentState == SECTION_HEADER_ACTIVE) {
            commentState = CIRCUIT_HEADER;
            if (!sectionHeader.isEmpty())
                startNewSection(sectionHeader);
            sectionHeader = "";
        }
        else {
            commentState = SECTION_HEADER_ACTIVE;
            sectionHeader = "";
        }
    }

    // Normal comment goes here...
    else {
        // Is this the very first comment in the patch? Then
        // it is considered to be the title. One line.
        // Exception: It is a meta comment "CONTROLLER 2:"
        // in a patch that does not have a title
        if (commentState == AWAITING_TITLE_COMMENT) {
            if (!maybeParseMetaComment(comment))
                patch->setTitle(comment);
            commentState = DESCRIPTION; // now we wait for the description
        }

        // We are already in the description? Add that line to the
        // description (or such register or meta data from that line)
        else if (commentState == DESCRIPTION) {
            if (!maybeParseRegisterComment(comment)
                && !maybeParseMetaComment(comment))
                currentComment.append(comment);
        }

        // A comment int the section header belongs to the name
        // of the section
        else if (commentState == SECTION_HEADER_ACTIVE) {
            if (!sectionHeader.isEmpty())
                sectionHeader += " ";
            sectionHeader += comment;
        }

        // Finally: collect normal comment for the next circuit
        else
            currentComment.append(comment);
    }
}
bool PatchParser::maybeParseRegisterComment(QString comment)
{
    // Examples:
    // I1: [CLK] optional external clock
    // P2.4: This is a comment without short

    static QRegularExpression regex("^([a-zA-Z])([1-9][0-9]*)[.]?([1-9][0-9]*)?:[[:space:]]*(.*)$");
    static QRegularExpression shorthand("^\\[([^]]+)\\][[:space:]]*(.*)$");

    QRegularExpressionMatch m;
    m = regex.match(comment);
    if (m.hasMatch()) {
        QChar registerName = m.captured(1).toUpper()[0];
        unsigned controller;
        unsigned number;
        if (m.captured(3).isEmpty()) {
            controller = 0;
            number = m.captured(2).toUInt();
        }
        else {
            controller = m.captured(2).toUInt();
            number = m.captured(3).toUInt();
        }
        QString atomcomment = m.captured(4);
        QRegularExpressionMatch m2;
        m2 = shorthand.match(atomcomment);
        QString shorthand;
        if (m2.hasMatch()) { // Format with short hand like [CLK]
            shorthand = m2.captured(1);
            atomcomment = m2.captured(2);
        }
        register_type_t registerType = registerName.toLatin1();
        patch->addRegisterComment(registerType, controller, number, shorthand, atomcomment);
        return true;
    }
    else
        return false;
}
bool PatchParser::maybeParseMetaComment(QString comment)
{
    static QRegularExpression regex("^[[:space:]]*([A-Z][A-Z 0-9]*):[[:space:]]*(.*)$");
    QRegularExpressionMatch m;
    m = regex.match(comment);
    if (m.hasMatch()) {
        if (m.captured(1) == "LIBRARY")
            parseLibraryMetaData(m.captured(2));
        // Ignore the other headers. They are most probably from
        // the output of the registers like "INPUTS:"
        return true;
    }
    else
        return false;
}
void PatchParser::parseLibraryMetaData(QString data)
{
    // Example:
    // LIBRARY: name=arpeggio; version=1.0; firmware=blue-1
    patch->setLibraryMetaData(data);
}
void PatchParser::parseCircuitLine(QString line, bool disabled)
{
    static QRegularExpression withComment("^\\[([a-zA-Z0-9]+)\\][[:space:]]*#(.*)$");
    static QRegularExpression withoutComment("^\\[([a-zA-Z0-9]+)\\]$");

    QString circuitName;
    QString comment;

    QRegularExpressionMatch m;
    m = withComment.match(line);
    if (m.hasMatch()) {
        circuitName = m.captured(1);
        comment = m.captured(2);
    }
    else {
        m = withoutComment.match(line);
        if (m.hasMatch())
            circuitName = m.captured(1);
        else {
            throw ParseException("Invalid line starting with [");
        }
    }

    if (ModuleBuilder::controllerExists(circuitName.toLower())) {
        if (!disabled) { // disabling controller is not supported
            patch->addController(circuitName.toLower());
            currentComment.clear();
        }
    }
    else {
        commentState = CIRCUIT_HEADER;
        if (comment != "")
            currentComment.append(comment);
        parseCircuit(circuitName, disabled);
    }
}
void PatchParser::parseCircuit(QString name, bool disabled)
{
    QString namel = name.toLower();

    static QRegularExpression e("^[a-z][a-z0-9]+$");
    if (!e.match(namel).hasMatch())
        throw ParseException("Invalid circuit name '" + name + "'");

    if (!section)
        startNewSection("");

    stripEmptyCommentLines();
    circuit = new Circuit(name, currentComment, disabled);
    section->addCircuit(circuit);
    currentComment.clear();
}
void PatchParser::stripEmptyCommentLines()
{
    while (currentComment.size() > 0 &&
           currentComment.last().isEmpty())
    {
        currentComment.removeLast();
    }
}
void PatchParser::startNewSection(QString name)
{
    section = new PatchSection(name);
    patch->addSection(section);
}
void PatchParser::parseJackLine(Circuit *circuit, QString line, bool disabled)
{
    JackAssignment *ja = JackAssignment::parseJackLine(circuit->getName(), line);
    ja->setDisabled(disabled);
    circuit->addJackAssignment(ja);
}
