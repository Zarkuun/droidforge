#include "mainwindow.h"
#include "patchparser.h"
#include "QtCore/qdebug.h"
#include "modulebuilder.h"

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

    JackAssignment *ja = new JackAssignment();
    if (parts.size() > 1) {
        ja->comment = parts.mid(1).join('#').trimmed();
    }

    parts = parts[0].split("=");

    ja->jack = parts[0].trimmed().toLower();

    if (the_firmware->jackIsInput(circuit->name, ja->jack))
        ja->jackType = JACKTYPE_INPUT;

    else if (the_firmware->jackIsOutput(circuit->name, ja->jack))
        ja->jackType = JACKTYPE_OUTPUT;

    else {
        ja->jackType = JACKTYPE_UNKNOWN;
    }

    ja->parseSourceString(parts[1].trimmed());
    circuit->addJackAssignment(ja);
    return true;
}
