#include "patchparser.h"
#include "modulebuilder.h"
#include "parseexception.h"

#include <QTextStream>
#include <QRegularExpression>
#include <QException>

QRegularExpression sectionSeparator("^----*$");


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
        patch->addDescriptionLines(circuitComment);
        circuitComment.clear();
    }

    else if (circuitComment.size() > 0) {
        circuitComment.append("");
    }
}
void PatchParser::parseCommentLine(QString line)
{
    static QRegularExpression disabledJackLine("^[a-zA-z]+([1-9][0-9]*)?[[:space:]]*=.*");

    QString comment = line.mid(1).trimmed(); // strip off first '#'

    // Is this a disabled circuit?
    if (comment.startsWith("[")) {
        parseCircuitLine(comment, true /* disabled */);
        return;
    }

    // Or maybe a disabled jack line? (If there *is* already a circuit ongoing)
    if (circuit) {
        QRegularExpressionMatch m = disabledJackLine.match(comment);
        if (m.hasMatch()) {
            parseJackLine(circuit, comment, true /* disabled */);
            return;
        }
    }

    // Comments like "# ------------" start (or end) section headers
    if (sectionSeparator.match(comment).hasMatch()) {
        if (commentState == SECTION_HEADER_ACTIVE) {
            commentState = CIRCUIT_HEADER;
            if (!sectionHeader.isEmpty())
                startNewSection(sectionHeader, sectionComment);
            sectionHeader = "";
        }
        else {
            commentState = SECTION_HEADER_ACTIVE;
            sectionHeader = "";
            sectionComment.clear();
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
            if (!maybeParseRegisterLabel(comment)
                && !maybeParseMetaComment(comment))
                circuitComment.append(comment);
        }

        // The first line in a section header is its title,
        // all remaining lines are a section comment.
        else if (commentState == SECTION_HEADER_ACTIVE) {
            if (sectionHeader.isEmpty())
                sectionHeader = comment;
            else
                sectionComment.append(comment);
        }

        // Finally: collect normal comment for the next circuit
        else
            circuitComment.append(comment);
    }
}
bool PatchParser::maybeParseRegisterLabel(QString comment)
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
        unsigned controller = 0;
        unsigned g8 = 0;
        unsigned number = 0;
        if (m.captured(3).isEmpty()) {
            controller = 0;
            g8 = 0;
            number = m.captured(2).toUInt();
        }
        else {
            number = m.captured(3).toUInt();
            if (registerName == 'G' && number >= 1 && number <= 8)
                g8 = m.captured(2).toUInt();
            else
                controller = m.captured(2).toUInt();
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
        // Convert old-style registers like "G8" to new ones like "G1.8"
        if (registerType == 'G' && g8 == 0 && number <= 8)
            g8 = 1;
        patch->addRegisterLabel(registerType, controller, g8, number, shorthand, atomcomment);
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
        if (m.captured(1) == "LABELS")
            parseLabels(m.captured(2));
        // Ignore the other headers. They are most probably from
        // the output of the registers like "INPUTS:"
        return true;
    }
    else
        return false;
}
void PatchParser::parseLabels(QString data)
{
    static QRegularExpression label_re("^([a-zA-Z0-9]+)[[:space:]]*=(.*)$");

    // Example:
    // master=18; version=1.0; firmware=blue-1
    QStringList parts = data.split(";");
    for (auto part: parts) {
        auto matches = label_re.match(part.trimmed());
        if (matches.hasMatch()) {
            QString label = matches.captured(1);
            QString value = matches.captured(2);
            patch->setLabel(label, value);
        }
    }
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
            circuitComment.clear();
        }
    }
    else {
        commentState = CIRCUIT_HEADER;
        if (comment != "")
            circuitComment.append(comment);
        parseCircuit(circuitName, disabled);
    }
}
void PatchParser::parseCircuit(QString name, bool disabled)
{
    QString namel = name.toLower();

    static QRegularExpression e("^[a-z][a-z0-9]+$");
    if (!e.match(namel).hasMatch())
        throw ParseException("Invalid circuit name '" + name + "'");

    if (!section) {
        QStringList emptyComment;
        startNewSection("", emptyComment);
    }

    stripEmptyCommentLines();
    circuit = new Circuit(name, circuitComment, disabled);
    section->addCircuit(circuit);
    circuitComment.clear();
}
void PatchParser::stripEmptyCommentLines()
{
    while (circuitComment.size() > 0 &&
           circuitComment.last().isEmpty())
    {
        circuitComment.removeLast();
    }
}
void PatchParser::startNewSection(const QString &name, const QStringList &comment)
{
    section = new PatchSection(name);
    section->setComment(comment);
    patch->addSection(section);
}
void PatchParser::parseJackLine(Circuit *circuit, QString line, bool disabled)
{
    JackAssignment *ja = JackAssignment::parseJackLine(circuit->getName(), line);
    ja->setDisabled(disabled);
    circuit->addJackAssignment(ja);
}
