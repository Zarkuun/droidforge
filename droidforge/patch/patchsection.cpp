#include "patchsection.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "tuning.h"


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


QString PatchSection::toString() const
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

QString PatchSection::getNonemptyTitle() const
{
    if (title.isEmpty())
        return SECTION_DEFAULT_NAME;
    else
        return title;
}


void PatchSection::deleteCurrentCircuit()
{
    deleteCircuit(cursor.circuitNr);
}

void PatchSection::deleteCircuit(int circuitNumber)
{
    Circuit *c = circuits[circuitNumber];
    circuits.remove(circuitNumber);
    delete c;
    sanitizeCursor();
}

void PatchSection::deleteCurrentJackAssignment()
{
    Circuit *circuit = currentCircuit();
    circuit->deleteJackAssignment(cursor.row);
    sanitizeCursor();
}

void PatchSection::deleteCurrentComment()
{
    Circuit *circuit = currentCircuit();
    circuit->removeComment();
    sanitizeCursor();
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
        else {
            cursor.row = currentCircuit()->numJackAssignments() - 1;
            if (cursor.row == -1 && !currentCircuit()->hasComment())
                cursor.row --;
        }
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
            if (cursor.row == -1 && !currentCircuit()->hasComment())
                cursor.row--; // skip back to title
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

void PatchSection::sanitizeCursor()
{
    if (cursor.circuitNr >= circuits.size()) {
        cursor.circuitNr = circuits.size() - 1;
        cursor.column = 0;
        cursor.row = -2;
        return;
    }

    Circuit *circuit = currentCircuit();

    if (cursor.row >= circuit->numJackAssignments())
        cursor.row = circuit->numJackAssignments() - 1;

    else if (cursor.row == -1 && !circuit->hasComment())
        cursor.row = -2;

}

void PatchSection::moveCursorToNextCircuit()
{
    if (cursor.circuitNr < circuits.size()-1) {
        cursor.circuitNr ++;
        cursor.row = -2;
    }
}


void PatchSection::moveCursorToPreviousCircuit()
{
    if (cursor.row > -2)
        cursor.row = -2;
    else if (cursor.circuitNr > 0)
        cursor.circuitNr --;
}

void PatchSection::addNewCircuit(QString name, jackselection_t jackSelection)
{
    int newPosition;
    if (!circuits.isEmpty()) {
        newPosition = cursorPosition().circuitNr;
        if (cursorPosition().row != -2)
            newPosition ++;
    }
    else
        newPosition = 0;

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
    circuits.insert(newPosition, circuit);
    cursor.row = -2;
    cursor.circuitNr = newPosition;
}

void PatchSection::addCircuit(int pos, Circuit *circuit)
{
    circuits.insert(pos, circuit);
}

void PatchSection::addCircuit(Circuit *circuit)
{
    circuits.append(circuit);
}

void PatchSection::collectCables(QStringList &cables) const
{
    for (auto circuit: circuits)
        circuit->collectCables(cables);
}

void PatchSection::findCableConnections(const QString &cable, int &asInput, int &asOutput) const
{
    for (auto circuit: circuits)
        circuit->findCableConnections(cable, asInput, asOutput);
}

QList<PatchProblem *> PatchSection::collectProblems(const Patch *patch) const
{
    QList<PatchProblem *> allProblems;

    int circuitNr=0;
    for (auto circuit: circuits) {
        for (auto problem: circuit->collectProblems(patch)) {
            problem->setCircuit(circuitNr);
            allProblems.append(problem);
        }
        circuitNr++;
    }
    return allProblems;
}

Circuit *PatchSection::currentCircuit()
{
    if (circuits.size())
        return circuits[cursor.circuitNr];
    else
        return 0;
}

const Circuit *PatchSection::currentCircuit() const
{
    if (circuits.size())
        return circuits[cursor.circuitNr];
    else
        return 0;
}

const Atom *PatchSection::atomAt(const CursorPosition &pos)
{
    return circuits[pos.circuitNr]->atomAt(pos.row, pos.column);
}

void PatchSection::setAtomAt(const CursorPosition &pos, Atom *atom)
{
    circuits[pos.circuitNr]->setAtomAt(pos.row, pos.column, atom);

}

JackAssignment *PatchSection::currentJackAssignment()
{
    Circuit *c = currentCircuit();
    if (!c)
        return 0;

    if (cursor.row < 0)
        return 0;
    else
        return c->jackAssignment(cursor.row);
}

const JackAssignment *PatchSection::currentJackAssignment() const
{
    const Circuit *c = currentCircuit();
    if (!c)
        return 0;

    if (cursor.row < 0)
        return 0;
    else
        return c->jackAssignment(cursor.row);
}

bool PatchSection::needG8() const
{
    for (qsizetype i=0; i<circuits.length(); i++)
        if (circuits[i]->needG8())
            return true;
    return false;
}

bool PatchSection::needX7() const
{
    for (auto circuit: circuits)
        if (circuit->needX7())
            return true;
    return false;
}

void PatchSection::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (auto circuit: circuits)
        circuit->swapControllerNumbers(fromNumber, toNumber);
}

void PatchSection::shiftControllerNumbers(int number, int by)
{
    for (auto circuit: circuits)
        circuit->shiftControllerNumbers(number, by);
}

void PatchSection::collectRegisterAtoms(RegisterList &sl) const
{
    for (auto circuit: circuits)
        circuit->collectRegisterAtoms(sl);
}

void PatchSection::remapRegister(AtomRegister from, AtomRegister to)
{
    for (auto circuit: circuits)
        circuit->remapRegister(from, to);
}

void PatchSection::removeRegisterReferences(RegisterList &rl, int ih, int oh)
{
    for (auto circuit: circuits)
        circuit->removeRegisterReferences(rl, ih, oh);
}
