#include "patchsection.h"
#include "clipboard.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "tuning.h"
#include "globals.h"


PatchSection::PatchSection() : selection(0)
{
}
PatchSection::PatchSection(QString t) : title(t) , selection(0)
{
}
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

QString PatchSection::toBare() const
{
    QString s;
    for (qsizetype i=0; i<circuits.length(); i++)
        s += circuits[i]->toBare();
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
    cursor.row --; // 1 -3 0
    if (cursor.row == -1 && !currentCircuit()->hasComment())
        cursor.row --;

    if (cursor.row < -2) { // move up to previous circuit
        cursor.circuitNr--; // 0 -3 0
        if (cursor.circuitNr < 0) { // first circuit
            cursor.circuitNr = 0;
            cursor.row = -2;
        }

        else if (currentCircuit()->isFolded())
            cursor.row = -2;

        else {
            cursor.row = currentCircuit()->numJackAssignments() - 1;
            if (cursor.row == -1 && !currentCircuit()->hasComment())
                cursor.row --;
        }
    }
}
void PatchSection::moveCursorDown()
{
    if (currentCircuit()->isFolded()) {
        cursor.circuitNr = qMin(circuits.count() - 1, cursor.circuitNr+1);
        return;
    }

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

void PatchSection::setCursorRowColumn(int row, int column)
{
    cursor.row = row;
    cursor.column = column;
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
    else if (cursor.circuitNr < 0)
        cursor.circuitNr = 0;

    Circuit *circuit = currentCircuit();
    if (!circuit)
        return;

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
    Circuit *circuit = new Circuit(name, emptyComment, false /* disabled */);

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
void PatchSection::insertCircuit(int pos, Circuit *circuit)
{
    circuits.insert(pos, circuit);
}
void PatchSection::addCircuit(Circuit *circuit)
{
    circuits.append(circuit);
}
bool PatchSection::allCircuitsFolded() const
{
    for (auto circuit: circuits) {
        if (!circuit->isFolded())
            return false;
    }
    return true;
}
void PatchSection::toggleFold()
{
    bool fold = !allCircuitsFolded();
    for (auto circuit: circuits)
        circuit->setFold(fold);
    if (currentCircuit()->isFolded())
        setCursorRowColumn(-2, 0);
}
Patch *PatchSection::getSelectionAsPatch() const
{
    Clipboard cb;
    cb.copyFromSelection(selection, this);
    return cb.getAsPatch();
}
void PatchSection::clearSelection()
{
    if (selection) {
        delete selection;
        selection = 0;
    }
}
void PatchSection::selectAll()
{
    if (selection)
        delete selection;
    if (isEmpty())
        return;

    CursorPosition start(0, 0, 0);
    CursorPosition end(circuits.count()-1, circuits[circuits.count()-1]->numJackAssignments()-1, 0);
    selection = new Selection(start, end);
}
void PatchSection::setMouseSelection(const CursorPosition &to)
{
    if (selection)
        delete selection;
    selection = new Selection(cursor, to);
}
void PatchSection::updateKeyboardSelection(const CursorPosition &before, const CursorPosition &after)
{
    if (selection) {
        CursorPosition from, to;
        if (selection->fromPos() == before) {
            from = after;
            to = selection->toPos();
        }
        else {
            from = selection->fromPos();
            to = after;
        }
        delete selection;
        selection = new Selection(from, to);
    }
    else
        selection = new Selection(before, after);
}
void PatchSection::collectCables(QStringList &cables) const
{
    for (auto circuit: circuits)
        circuit->collectCables(cables);
}
void PatchSection::findCableConnections(const QString &cable, int &asInput, int &asOutput) const
{
    for (auto circuit: circuits)
        if (!circuit->isDisabled())
            circuit->findCableConnections(cable, asInput, asOutput);
}
QList<PatchProblem *> PatchSection::collectProblems(const Patch *patch) const
{
    QList<PatchProblem *> allProblems;

    int circuitNr=0;
    for (auto circuit: circuits) {
        if (circuit->isDisabled()) {
            circuitNr++;
            continue; // does not count
        }
        for (auto problem: circuit->collectProblems(patch)) {
            problem->setCircuit(circuitNr);
            allProblems.append(problem);
        }
        circuitNr++;
    }
    return allProblems;
}

void PatchSection::setCursor(const CursorPosition &pos)
{
    cursor = pos;
    if (pos.row != -2 && currentCircuit()->isFolded())
        currentCircuit()->setFold(false);
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
const Atom *PatchSection::currentAtom() const
{
    const Circuit *circuit = currentCircuit();
    if (circuit)
        return circuit->atomAt(cursor.row, cursor.column);
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
void PatchSection::swapCircuits(int ida, int idb)
{
    // Cursor sticks to the moved circuit
    if (cursor.circuitNr == ida)
        cursor.circuitNr = idb;
    else if (cursor.circuitNr == idb)
        cursor.circuitNr = ida;
    circuits.swapItemsAt(ida, idb);
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
JackAssignment *PatchSection::jackAssignmentAt(const CursorPosition &pos)
{
    if (pos.row >= 0)
    {
        Circuit *circuit = circuits[pos.circuitNr];
        return circuit->jackAssignment(pos.row);
    }
    else
        return 0;
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
    for (auto circuit: circuits) {
        if (!circuit->isDisabled())
            circuit->collectRegisterAtoms(sl);
    }
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
