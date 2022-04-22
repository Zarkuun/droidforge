#include "mainwindow.h"
#include "patchparser.h"
#include "modulebuilder.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "generalparseexception.h"

#include "QtCore/qdebug.h"
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
    errorMessage = "";
    errorLine = 0;
    section = 0;

    this->patch = patch;

    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        throw GeneralParseException("Cannot open file: ...");
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        errorLine ++;
        try {
            parseLine(line);
        }
        catch(QException &e) {
            qDebug() << "Mist isses in " << errorLine;
            throw GeneralParseException("Fehler in Zeiol;e|");
        }
    }
    inputFile.close();
}


bool PatchParser::parseLine(QString line)
{
    line = line.trimmed();
    if (line.isEmpty())
        return parseEmptyLine();
    else if (line[0] == '#')
        return parseCommentLine(line);
    else if (line[0] == '[')
        return parseCircuitLine(line);
    else if (line[0].isLetter() && line.contains('=')) {
        if (!circuit) {
            errorMessage = "Jack assignment before any circuit was declared.";
            return false;
        }
        else
            return parseJackLine(line);
    }
    else {
        errorMessage = "Garbled line";
        return false;
    }
}


bool PatchParser::parseEmptyLine()
{
    if (currentComment.size() > 0)
        currentComment.append("");
    return true;
}


bool PatchParser::parseCommentLine(QString line)
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
        if (commentState == SECTION_HEADER_ACTIVE) {
            if (!sectionHeader.isEmpty())
                sectionHeader += " ";
            sectionHeader += comment;
        }
        else
            currentComment.append(comment);
    }
    return true;
}


bool PatchParser::parseCircuitLine(QString line)
{
    commentState = CIRCUIT_HEADER;

    if (!line.endsWith(']')) {
        errorMessage = "Missing ] at the end of the line";
        return false;
    }

    QString circuitName = line.sliced(1).chopped(1);

    if (parseController(circuitName)) {
        currentComment.clear();
        return true;
    }
    else if (parseCircuit(circuitName))
        return true;

    errorMessage = "Invalid controller or circuit name '" + circuitName + "'";
    return false;
}


bool PatchParser::parseController(QString name)
{
    if (ModuleBuilder::controllerExists(name.toLower())) {
        patch->addController(name.toLower());
        return true;
    }
    else
        return false;
}


bool PatchParser::parseCircuit(QString name)
{
    // TODO: Hier die JSON-Datei verwenden?
    QString namel = name.toLower();

    static QRegularExpression e("^[a-z][a-z0-9]+$");
    if (!e.match(namel).hasMatch()) {
        errorMessage = "Invalid circuit name.";
        return false;
    }

    if (!section) {
        startNewSection("");
    }

    stripEmptyCommentLines();
    circuit = new Circuit(name, currentComment);
    section->circuits.append(circuit);
    currentComment.clear();
    return true;
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
    patch->sections.append(section);
}


bool PatchParser::parseJackLine(QString line)
{
    QStringList parts = line.split("#");
    if (parts[0].count('=') != 1) {
        errorMessage = "Duplicate =";
        return false;
    }

    QString comment;
    if (parts.size() > 1)
        comment = parts.mid(1).join('#').trimmed();

    parts = parts[0].split("=");
    QString jack = parts[0].trimmed().toLower();
    QString valueString = parts[1].trimmed();

    JackAssignment *ja;

    if (the_firmware->jackIsInput(circuit->getName(), jack))
        ja = new JackAssignmentInput(jack, comment, valueString);
    else if (the_firmware->jackIsOutput(circuit->getName(), jack))
        ja = new JackAssignmentOutput(jack, comment, valueString);
    else
        ja = new JackAssignmentUnknown(jack, comment, valueString);
    circuit->addJackAssignment(ja);
    return true;
}
