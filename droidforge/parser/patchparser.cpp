#include "mainwindow.h"
#include "patchparser.h"
#include "modulebuilder.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "parseexception.h"
#include "registercomments.h"

#include <QTextStream>
#include <QRegularExpression>
#include <QException>


PatchParser::PatchParser()
    : patch(0)
    , section(0)
    , circuit(0)
    , commentState(AWAITING_TITLE_COMMENT)
{
}


void PatchParser::parse(QString fileName, Patch *patch)
{
    QFileInfo fi(fileName);
    patch->setFileName(fi.baseName());
    errorMessage = "";
    errorLine = 0;
    section = 0;

    this->patch = patch;

    QString lineerrors;

    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        throw ParseException("Cannot open file: " + inputFile.errorString());
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        errorLine ++;
        try {
            parseLine(line);
        }
        catch (ParseException &e) {
            QString error = QString("Line ") + QString::number(errorLine) + ": " + e.toString() + "\n";
            lineerrors += error;
        }
    }
    inputFile.close();
    if (!lineerrors.isEmpty())
        throw ParseException("Syntax errors in your patch:\n\n" + lineerrors);
}


void PatchParser::parseLine(QString line)
{
    line = line.trimmed();
    if (line.isEmpty())
        parseEmptyLine();
    else if (line[0] == '#')
        parseCommentLine(line);
    else if (line[0] == '[')
        parseCircuitLine(line);
    else if (line[0].isLetter() && line.contains('=')) {
        if (!circuit)
            throw ParseException("Jack assignment before any circuit was declared.");
        else
            parseJackLine(circuit, line);
    }
    else
        throw ParseException("Garbled line");
}


void PatchParser::parseEmptyLine()
{
    if (currentComment.size() > 0)
        currentComment.append("");
}


void PatchParser::parseCommentLine(QString line)
{
    QString comment = line.mid(1).trimmed();
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
    else {
        if (commentState == AWAITING_TITLE_COMMENT) {
            patch->setTitle(comment);
            commentState = DESCRIPTION;
        }
        else if (commentState == DESCRIPTION) {
            if (!maybeParseRegisterComment(comment)
                && !maybeParseMetaComment(comment))
            patch->addDescriptionLine(comment);
        }
        else if (commentState == SECTION_HEADER_ACTIVE) {
            if (!sectionHeader.isEmpty())
                sectionHeader += " ";
            sectionHeader += comment;
        }
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
        patch->addRegisterComment(registerName, controller, number, shorthand, atomcomment);
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


void PatchParser::parseCircuitLine(QString line)
{
    commentState = CIRCUIT_HEADER;

    if (!line.endsWith(']'))
        throw ParseException("Missing ] at the end of the line");

    QString circuitName = line.sliced(1).chopped(1);

    if (ModuleBuilder::controllerExists(circuitName.toLower())) {
        patch->addController(circuitName.toLower());
        currentComment.clear();
    }
    else
        parseCircuit(circuitName);
}

void PatchParser::parseCircuit(QString name)
{
    // TODO: Hier die JSON-Datei verwenden?
    QString namel = name.toLower();

    static QRegularExpression e("^[a-z][a-z0-9]+$");
    if (!e.match(namel).hasMatch())
        throw ParseException("Invalid circuit name '" + name + "'");

    if (!section)
        startNewSection("");

    stripEmptyCommentLines();
    circuit = new Circuit(name, currentComment);
    section->circuits.append(circuit);
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


void PatchParser::parseJackLine(Circuit *circuit, QString line)
{
    JackAssignment *ja = JackAssignment::parseJackLine(circuit->getName(), line);
    circuit->addJackAssignment(ja);
}