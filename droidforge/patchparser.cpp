#include "patchparser.h"
#include "QtCore/qdebug.h"

#include <QTextStream>

PatchParser::PatchParser()
    : patch(0)
{
}


bool PatchParser::parse(QString fileName, Patch *patch)
{
    errorMessage = "";
    errorLine = 0;

    this->patch = patch;

    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        errorMessage = "Cannot open file.";
        return false;
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        parseLine(line);
    }
    inputFile.close();
    return true;
}


void PatchParser::parseLine(QString line)
{
    qDebug() << "Parse zeile: " << line;
}
