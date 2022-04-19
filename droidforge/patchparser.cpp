#include "mainwindow.h"
#include "patchparser.h"
#include "modulebuilder.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"

#include "QtCore/qdebug.h"
#include <QTextStream>
#include <QRegularExpression>


PatchParser::PatchParser()
    : patch(0)
    , section(0)
    , circuit(0)
{
}


bool PatchParser::parse(QString fileName, Patch *patch)
{
    errorMessage = "";
    errorLine = 0;
    section = 0;

    this->patch = patch;

    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        errorMessage = "Cannot open file.";
        return false;
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        errorLine ++;
        if (!parseLine(line))
            return false;
    }
    inputFile.close();
    return true;
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
    return true;
}


bool PatchParser::parseCommentLine(QString)
{
    return true; // TODO: Comments
}


bool PatchParser::parseCircuitLine(QString line)
{
    if (!line.endsWith(']')) {
        errorMessage = "Missing ] at the end of the line";
        return false;
    }

    QString circuitName = line.sliced(1).chopped(1);

    if (parseController(circuitName))
        return true;
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
        section = new PatchSection("Circuits");
        patch->sections.append(section);
    }

    circuit = new Circuit(name);
    section->circuits.append(circuit);
    return true;
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

    if (the_firmware->jackIsInput(circuit->name, jack))
        ja = new JackAssignmentInput(jack, valueString);
    else if (the_firmware->jackIsOutput(circuit->name, jack))
        ja = new JackAssignmentOutput(jack, valueString);
    else
        ja = new JackAssignmentUnknown(jack, valueString);
    circuit->addJackAssignment(ja);
    return true;
}
