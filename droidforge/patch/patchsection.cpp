#include "patchsection.h"
#include "clipboard.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "tuning.h"
#include "globals.h"
#include "atomnumber.h"


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

QString PatchSection::toCleanString() const
{
    QString s;
    for (qsizetype i=0; i<circuits.length(); i++) {
        if (!circuits[i]->isDisabled()) {
            s += circuits[i]->toCleanString();
            s += "\n";
        }
    }
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
    cursor.row = ROW_CIRCUIT;
    sanitizeCursor();
}
void PatchSection::deleteCircuit(int circuitNumber)
{
    Circuit *c = circuits[circuitNumber];
    circuits.remove(circuitNumber);
    delete c;
    cursor.row = ROW_CIRCUIT;
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
    if (cursor.row == ROW_COMMENT && !currentCircuit()->hasComment())
        cursor.row --;

    if (cursor.row < ROW_CIRCUIT) { // move up to previous circuit
        cursor.circuitNr--; // 0 -3 0
        if (cursor.circuitNr < 0) { // first circuit
            cursor.circuitNr = 0;
            cursor.row = ROW_CIRCUIT;
        }

        else if (currentCircuit()->isFolded())
            cursor.row = ROW_CIRCUIT;

        else {
            cursor.row = currentCircuit()->numJackAssignments() - 1;
            if (cursor.row == ROW_COMMENT && !currentCircuit()->hasComment())
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
    if (cursor.row == ROW_COMMENT && !currentCircuit()->hasComment())
        cursor.row ++;
    if (cursor.row >= n) {
        cursor.circuitNr ++;
        if (cursor.circuitNr >= circuits.size()) {
            cursor.row--;
            cursor.circuitNr--;
            if (cursor.row == ROW_COMMENT && !currentCircuit()->hasComment())
                cursor.row--; // skip back to title
        }
        else
            cursor.row = ROW_CIRCUIT;
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

void PatchSection::moveCursorForward()
{
    CursorPosition pos = cursorPosition();
    while (true) {
        if (!nextCursorPosition(pos))
            return;
        if (pos.row == ROW_COMMENT || pos.column >= 1)
            break;
    }
    setCursor(pos);
}
bool PatchSection::nextCursorPosition(CursorPosition &pos) const
{
    const Circuit *circuit = circuits[pos.circuitNr];

    // Circuit without jack assignments
    if (circuit->numJackAssignments() == 0)
    {
        if (pos.row == ROW_CIRCUIT && circuit->hasComment()) {
            pos.row++;
            return true;
        }
        else
            return nextCircuitCursorPosition(pos);
    }

    // From header to comment or first jack assignment
    if (pos.row == ROW_CIRCUIT) {
        pos.row++;
        if (!circuit->hasComment())
            pos.row++;
        return true;
    }

    // From comment to first jack assignment
    if (pos.row == ROW_COMMENT) {
        pos.row++;
        return true;
    }

    // We have this jack assignment, because we start from
    // a valid cursor position
    const JackAssignment *ja = circuit->jackAssignment(pos.row);
    unsigned numAtoms = ja->isInput() ? 3 : 1;
    if (pos.column < (int)numAtoms) {
        pos.column ++;
        return true;
    }

    pos.column = 1;
    pos.row ++;
    if (pos.row >= circuit->numJackAssignments())
        return nextCircuitCursorPosition(pos);

    return true;
}
bool PatchSection::nextCircuitCursorPosition(CursorPosition &pos) const
{
    if (pos.circuitNr + 1 < (int)numCircuits())
    {
        pos.circuitNr ++;
        pos.row = ROW_CIRCUIT;
        pos.column = 0;
        return true;
    }
    else
        return false;
}
void PatchSection::moveCursorBackward()
{
    CursorPosition pos = cursorPosition();
    while (true) {
        if (!previousCursorPosition(pos))
            return;
        if (pos.row == ROW_COMMENT || pos.column >= 1)
            break;
    }
    setCursor(pos);
}
bool PatchSection::previousCursorPosition(CursorPosition &pos) const
{
    if (pos.column > 0) {
        pos.column--;
        return true;
    }

    const Circuit *circuit = circuits[pos.circuitNr];

    if (pos.row > 0) {
        pos.row--;
        const JackAssignment *ja = circuit->jackAssignment(pos.row);
        pos.column = ja->isInput() ? 3 : 1;
        return true;
    }

    // We start at row 0? Either to comment or header
    if (pos.row == 0) {
        if (circuit->hasComment())
            pos.row = ROW_COMMENT;
        else
            pos.row = ROW_CIRCUIT;
        return true;
    }

    // We start at comment -> go to header
    if (pos.row == ROW_COMMENT) {
        pos.row = ROW_CIRCUIT;
        return true;
    }

    // We start at header -> previous circuit
    if (pos.circuitNr == 0)
        return false;

    pos.circuitNr--;
    circuit = circuits[pos.circuitNr];

    if (circuit->numJackAssignments()) {
        pos.row = circuit->numJackAssignments() - 1;
        const JackAssignment *ja = circuit->jackAssignment(pos.row);
        pos.column = ja->isInput() ? 3 : 1;
        return true;
    }
    else if (circuit->hasComment()) {
        pos.row = ROW_COMMENT;
        pos.column = 0;
        return true;
    }
    else {
        pos.row = ROW_CIRCUIT;
        pos.column = 0;
        return true;
    }
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
        cursor.row = ROW_CIRCUIT;
        return;
    }
    else if (cursor.circuitNr < 0)
        cursor.circuitNr = 0;

    Circuit *circuit = currentCircuit();
    if (!circuit)
        return;

    else if (cursor.row >= circuit->numJackAssignments())
        cursor.row = circuit->numJackAssignments() - 1;

    if (cursor.row == ROW_COMMENT && !circuit->hasComment())
        cursor.row = ROW_CIRCUIT;

}

CursorPosition PatchSection::canonizedCursorPosition(const CursorPosition &pos) const
{
    if (pos.row < 0)
        return pos;

    Circuit *circuit = circuits[pos.circuitNr];
    JackAssignment *ja = circuit->jackAssignment(pos.row);
    if (ja->isInput())
        return pos;
    else {
        CursorPosition can = pos;
        can.column = qMin(pos.column, 1);
        return can;
    }
}
void PatchSection::moveCursorToNextCircuit()
{
    if (cursor.circuitNr < circuits.size()-1) {
        cursor.circuitNr ++;
        cursor.row = ROW_CIRCUIT;
    }
}
void PatchSection::moveCursorToPreviousCircuit()
{
    if (cursor.row > ROW_CIRCUIT)
        cursor.row = ROW_CIRCUIT;
    else if (cursor.circuitNr > 0)
        cursor.circuitNr --;
}
void PatchSection::addNewCircuit(QString name, jackselection_t jackSelection, int position)
{
    int newPosition;
    if (position != -1)
        newPosition = position;
    else if (!circuits.isEmpty()) {
        newPosition = cursorPosition().circuitNr;
        if (cursorPosition().row != ROW_CIRCUIT)
            newPosition ++;
    }
    else
        newPosition = 0;

    QStringList emptyComment;
    Circuit *circuit = new Circuit(name, emptyComment, false /* disabled */);

    QStringList ei = the_firmware->inputsOfCircuit(name, jackSelection);
    for (auto &jackName: ei) {
        auto ja = new JackAssignmentInput(jackName);
        if (the_firmware->jackHasDefaultvalue(name, jackName)) {
            AtomNumber *an = new AtomNumber(the_firmware->jackDefaultvalue(name, jackName), ATOM_NUMBER_NUMBER);
            ja->replaceAtom(1, an);
        }
        circuit->addJackAssignment(ja);
    }
    QStringList eo = the_firmware->outputsOfCircuit(name, jackSelection);
    for (qsizetype o=0; o<eo.count(); o++) {;
        circuit->addJackAssignment(new JackAssignmentOutput(eo[o]));
    }
    circuits.insert(newPosition, circuit);
    cursor.row = ROW_CIRCUIT;
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
        setCursorRowColumn(ROW_CIRCUIT, 0);
}
void PatchSection::rewriteSelectedCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode)
{
    if (selection->isCircuitSelection()) {
        for (int i=selection->fromPos().circuitNr;
             i<=selection->toPos().circuitNr;
             i++)
        {
            Circuit *circuit = circuits[i];
            circuit->rewriteCableNames(remove, insert, mode);
        }
    }
    else if (selection->isJackSelection()) {
        Circuit *circuit = circuits[selection->fromPos().circuitNr];
        circuit->rewriteCableNames(remove, insert, mode, selection->fromPos().row, selection->toPos().row);
    }
    else if (selection->isSingleAtomSelection()) {
        Atom *atom = atomAt(selection->fromPos());
        atom->rewriteCableNames(remove, insert, mode);
    }
    else if (selection->isAtomSelection()) {
        for (int i=selection->fromPos().column;
             i<=selection->toPos().column;
             i++)
        {
            CursorPosition pos(selection->fromPos().circuitNr,
                               selection->fromPos().row,
                               i);
            Atom *atom = atomAt(pos);
            if (atom)
                atom->rewriteCableNames(remove, insert, mode);
        }
    }
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
    if (selection) {
        delete selection;
        selection = 0;
    }

    if (isEmpty())
        return;

    CursorPosition start(0, ROW_CIRCUIT, 0);
    CursorPosition end(
                circuits.count() - 1,
                circuits[circuits.count() - 1]->numJackAssignments() -1,
                0);
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
    if (pos.row != ROW_CIRCUIT && currentCircuit()->isFolded())
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
    CursorPosition can = canonizedCursorPosition(cursor);
    const Circuit *circuit = currentCircuit();
    if (circuit)
        return circuit->atomAt(can.row, can.column);
    else
        return 0;
}
const Atom *PatchSection::atomAt(const CursorPosition &pos) const
{
    return circuits[pos.circuitNr]->atomAt(pos.row, pos.column);
}
Atom *PatchSection::atomAt(const CursorPosition &pos)
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
void PatchSection::collectRegisterAtoms(RegisterList &sl) const
{
    for (auto circuit: circuits) {
        if (!circuit->isDisabled())
            circuit->collectRegisterAtoms(sl);
    }
}
void PatchSection::removeRegisterReferences(RegisterList &rl)
{
    for (auto circuit: circuits)
        circuit->removeRegisterReferences(rl);
}
unsigned PatchSection::memoryFootprint() const
{
    unsigned memory = 0;
    for (auto circuit: circuits)
        memory += circuit->memoryFootprint();
    return memory;
}
bool PatchSection::needsX7() const
{
    for (auto circuit: circuits) {
        if (!circuit->isDisabled() && circuit->needsX7())
            return true;
    }
    return false;
}

bool PatchSection::searchHit(const QString &text)
{
    if (isEmpty())
        return false;

    QString refText;
    CursorPosition pos = cursorPosition();
    if (pos.row == ROW_CIRCUIT)
        refText = currentCircuit()->getName();
    else if (pos.row == ROW_COMMENT)
        refText = currentCircuit()->getComment();
    else {
        JackAssignment *ja = currentJackAssignment();
        if (pos.column == 0)
            refText = ja->jackName() + " " + ja->getComment();
        else {
            const Atom *atom = ja->atomAt(pos.column);
            if (!atom)
                return false;
            else
                refText = atom->toDisplay();
        }
    }

    return refText.contains(text, Qt::CaseInsensitive);
}
