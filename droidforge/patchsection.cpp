#include "patchsection.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"

#include <QDebug>


PatchSection::~PatchSection()
{
    for (qsizetype i=0; i<circuits.length(); i++)
        delete circuits[i];

}

PatchSection *PatchSection::clone() const
{
    PatchSection *newsection = new PatchSection(title);
    newsection->cursor = cursor;
    for (unsigned i=0; i<circuits.size(); i++)
        newsection->circuits.append(circuits[i]->clone());
    return newsection;
}


QString PatchSection::toString()
{
    QString s;

    if (!title.isEmpty()) {
        s += "# -------------------------------------------------\n";
        s += "# " + title + "\n";
        s += "# -------------------------------------------------\n\n";
    }

    for (qsizetype i=0; i<circuits.length(); i++)
        s += circuits[i]->toString();

    return s;
}


void PatchSection::deleteCurrentCircuit()
{
    Circuit *c = circuits[cursor.circuitNr];
    circuits.remove(cursor.circuitNr);
    delete c;
    if (cursor.circuitNr >= circuits.size())
        cursor.circuitNr--;
    cursor.column = 0;
    cursor.row = -2;
}


void PatchSection::deleteCurrentJackAssignment()
{
    Circuit *circuit = currentCircuit();
    circuit->deleteJackAssignment(cursor.row);
    if (cursor.row >= circuit->numJackAssignments())
        cursor.row = circuit->numJackAssignments() - 1;
}

void PatchSection::deleteCurrentComment()
{
    Circuit *circuit = currentCircuit();
    circuit->removeComment();
    cursor.row = -2;
}


void PatchSection::moveCursorUp()
{
    cursor.row --;
    if (cursor.row == -1 && !currentCircuit()->hasComment())
        cursor.row --;
    if (cursor.row < -2) {
        cursor.circuitNr--;
        if (cursor.circuitNr < 0) {
            cursor.circuitNr = 0;
            cursor.row = -2;
        }
        else
            cursor.row = currentCircuit()->numJackAssignments() - 1;
    }
}


void PatchSection::moveCursorDown()
{
    int n = currentCircuit()->numJackAssignments();
    cursor.row ++;
    if (cursor.row == -1 && !currentCircuit()->hasComment())
        cursor.row ++;
    if (cursor.row >= n) {
        cursor.circuitNr ++;
        if (cursor.circuitNr >= circuits.size()) {
            cursor.row--;
            cursor.circuitNr--;
        }
        else
            cursor.row = -2;
    }
}


void PatchSection::moveCursorLeft()
{
    // In rows with output jacks, always move the cursor
    // to column 0.
    if (cursor.row < 0)
        cursor.column = 0;

    else {
        JackAssignment *ja = currentCircuit()->jackAssignment(cursor.row);
        if (ja->jackType() != JACKTYPE_INPUT)
            cursor.column = 0;
        else {
            cursor.column --;
            if (cursor.column < 0)
                cursor.column = 0;
        }
    }
}


void PatchSection::moveCursorRight()
{
    cursor.column ++;
    if (cursor.column > 3)
        cursor.column = 3;
}

void PatchSection::setCursorRow(int row)
{
    cursor.row = row;
}

void PatchSection::setCursorColumn(int column)
{
    cursor.column = column;
}

void PatchSection::moveCursorToNextCircuit()
{
    if (cursor.circuitNr < circuits.size()-1) {
        cursor.circuitNr ++;
        cursor.row = -1;
    }
}


void PatchSection::moveCursorToPreviousCircuit()
{
    if (cursor.row > -1)
        cursor.row = -1;
    else if (cursor.circuitNr > 0)
        cursor.circuitNr --;
}


void PatchSection::addNewCircuit(int pos, QString name, jackselection_t jackSelection)
{
    QStringList emptyComment;
    Circuit *circuit = new Circuit(name, emptyComment);

    QStringList ei = the_firmware->inputsOfCircuit(name, jackSelection);
    for (qsizetype i=0; i<ei.count(); i++) {
        circuit->addJackAssignment(new JackAssignmentInput(ei[i])); // TODO: Default value
    }
    QStringList eo = the_firmware->outputsOfCircuit(name, jackSelection);
    for (qsizetype o=0; o<eo.count(); o++) {;
        circuit->addJackAssignment(new JackAssignmentOutput(eo[o]));
    }
    circuits.insert(pos, circuit);
    cursor.row = -2;
    cursor.circuitNr = pos;
}


void PatchSection::collectCables(QStringList &cables) const
{
    for (qsizetype i=0; i<circuits.length(); i++)
        circuits[i]->collectCables(cables);
}


Circuit *PatchSection::currentCircuit()
{
    return circuits[cursor.circuitNr];
}


JackAssignment *PatchSection::currentJackAssignment()
{
    if (cursor.row == -1)
        return 0;
    else
        return currentCircuit()->jackAssignment(cursor.row);
}
