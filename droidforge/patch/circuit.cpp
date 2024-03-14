#include "circuit.h"
#include "droidfirmware.h"
#include "jackassignmentoutput.h"
#include "patch.h"
#include "globals.h"

#include <QCoreApplication>
#include <QSettings>

#define tr(s) QCoreApplication::translate("Patch", s)

Circuit::Circuit(QString name, const QStringList &comment, bool disabled)
    : name(name)
    , comment(comment)
    , disabled(disabled)
    , folded(false)
    , haveBookmark(false)
{
}
Circuit::~Circuit()
{
    for (qsizetype i=0; i<jackAssignments.length(); i++)
        delete jackAssignments[i];
}
Circuit *Circuit::clone() const
{
    Circuit *newcircuit = new Circuit(name, comment, disabled);
    for (unsigned i=0; i<jackAssignments.size(); i++)
        newcircuit->jackAssignments.append(jackAssignments[i]->clone());
    newcircuit->folded = folded;
    return newcircuit;
}
void Circuit::addJackAssignment(JackAssignment *ja)
{
    jackAssignments.append(ja);
    if (isDisabled())
        ja->setDisabled(true);
}
void Circuit::insertJackAssignment(JackAssignment *ja, int index)
{
    jackAssignments.insert(index, ja);
    if (haveBookmark && bmRow >= index)
        bmRow++;
}
JackAssignment *Circuit::findJack(const QString name)
{
    for (auto ja: jackAssignments)
        if (ja->jackName() == name)
            return ja;
    return 0;
}
QString Circuit::prefixOfJack(const QString &jackName)
{
    QString prefix = jackName;
    while (prefix != "") {
        if (the_firmware->jackArraySize(name, prefix, true /* isInput */))
            return prefix;
        if (the_firmware->jackArraySize(name, prefix, false /* isInput */))
            return prefix;
        prefix.chop(1);
    }
    return jackName;
}
unsigned Circuit::memoryFootprint() const
{
    unsigned ram = isDisabled() ? 0 : the_firmware->circuitMemoryFootprint(name);
    for (auto ja: jackAssignments) {
        if (ja->isDisabled())
            ram += 0;
        else if (ja->jackType() == JACKTYPE_UNKNOWN)
            ram += 12; // just as a guess
        else
            ram += the_firmware->jackMemoryFootprint(name, ja->jackName());
    }
    return ram;
}
unsigned int Circuit::countDuplicateInputLines(QList<const JackAssignmentInput *> &inputLines) const
{
    // TODO: Some intelligent sorting
    unsigned count = 0;
    for (auto ja: jackAssignments) {
        if (!ja->isDisabled() && ja->isInput()) {
            if (the_firmware->jackMemoryFootprint(name, ja->jackName()) == 12) {
                const JackAssignmentInput *jai = (const JackAssignmentInput *)ja;
                bool found = false;
                for (auto j: inputLines) {
                    if (j->sameAs(jai)) {
                        count ++;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    inputLines.append(jai);
            }
        }
    }
    return count;
}
bool Circuit::needsMIDI() const
{
    return the_firmware->circuitNeedsMIDI(name);
}

bool Circuit::needsMASTER18() const
{
    return the_firmware->circuitNeedsMaster18(name);
}
bool Circuit::usesSelect() const
{
    for (auto ja: jackAssignments) {
        if (!ja->isDisabled() && ja->isInput() && ja->jackName() == "select")
            return true;
    }
    return false;
}
void Circuit::setBookmark(int row, int column)
{
     bmRow = row;
     bmColumn = column;
     haveBookmark = true;
}
const JackAssignment *Circuit::findJack(const QString name) const
{
    for (auto ja: jackAssignments)
        if (ja->jackName() == name)
            return ja;
    return 0;
}
bool Circuit::hasUndefinedJacks() const
{
    for (auto ja: jackAssignments)
        if (ja->isUndefined())
            return true;
    return false;
}
void Circuit::removeUndefinedJacks()
{
    for (qsizetype i=0; i<jackAssignments.count(); i++) {
        if (jackAssignments[i]->isUndefined()) {
            jackAssignments.remove(i);
            if (haveBookmark) {
                if (bmRow > i)
                    bmRow--;
                else if (bmRow == i)
                    clearBookmark();
            }
            i--;
        }
    }
}
QString Circuit::nextJackArrayName(const QString &jackName, bool isInput)
{
    QString prefix = prefixOfJack(jackName);
    unsigned thisIndex = jackName.mid(prefix.length()).toUInt();
    unsigned size = the_firmware->jackArraySize(name, prefix, isInput);
    if (size == 0)
        return "";

    QString next;
    for (unsigned i=thisIndex + 1; i <= size; i++) {
        next = prefix + QString::number(i);
        if (!findJack(next))
            return next;
    }
    return "";
}
void Circuit::deleteJackAssignment(unsigned i)
{
    delete jackAssignments[i];
    jackAssignments.remove(i);
    if (haveBookmark) {
        if (bmRow > (int)i)
            bmRow--;
        else if (bmRow == (int)i)
            clearBookmark();
    }
}
void Circuit::sortJacks()
{
    sortJacksFromTo(0, jackAssignments.count() - 1);
}
void Circuit::sortJacksFromTo(int fromRow, int toRow)
{
    // Bubble sort rulez
    bool sorted = false;
    while (!sorted) {
        sorted = true;
        for (int i=fromRow; i<toRow; i++) {
            if (*jackAssignments[i+1] < *jackAssignments[i]) {
                jackAssignments.swapItemsAt(i, i+1);
                if (haveBookmark) {
                    if (bmRow == i)
                        bmRow = i+1;
                    else if (bmRow == i+1)
                        bmRow = i;
                }
                sorted = false;
            }
        }
    }
}
const Atom *Circuit::atomAt(int row, int column) const
{
    if (row < 0 || row >= jackAssignments.count())
        return 0;
    else
        return jackAssignments[row]->atomAt(column);
}
Atom *Circuit::atomAt(int row, int column)
{
    if (row < 0 || row >= jackAssignments.count())
        return 0;
    else
        return jackAssignments[row]->atomAt(column);
}
void Circuit::setAtomAt(int row, int column, Atom *atom)
{
    jackAssignments[row]->replaceAtom(column, atom);
}
void Circuit::collectCables(QStringList &cables) const
{
    for (auto ja: jackAssignments)
        ja->collectCables(cables);
}
void Circuit::findCableConnections(const QString &cable, int &asInput, int &asOutput) const
{
    for (auto ja: jackAssignments)
        if (!ja->isDisabled())
            ja->findCableConnections(cable, asInput, asOutput);
}
void Circuit::rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode, int fromRow, int toRow)
{
    if (toRow == -1)
        toRow = jackAssignments.count() - 1;
    for (int row=fromRow; row <= toRow; row++) {
        JackAssignment *ja = jackAssignments[row];
        ja->rewriteCableNames(remove, insert, mode);
    }
}
QList<PatchProblem *> Circuit::collectProblems(const Patch *patch) const
{
    QSettings settings;
    QList<PatchProblem *> allProblems;

    if (!the_firmware->circuitExists(name)) {
        allProblems.append(
            new PatchProblem(ROW_CIRCUIT, 0, tr("There is no such circuit with the name '%1'").arg(name)));
    }


    if (the_firmware->circuitIsDeprecated(name) &&
        settings.value("validation/denounce_deprecated_circuits", true).toBool())
    {
        allProblems.append(
            new PatchProblem(ROW_CIRCUIT, 0, tr("This circuit is deprecated. It will be removed "
                                                "in future firmwares. You can disable this error"
                                                " in the preferences.")));
    }


    unsigned master = patch->typeOfMaster();
    if (master == 16 && needsMASTER18())
    {
        allProblems.append(
            new PatchProblem(ROW_CIRCUIT, 0, tr("The MASTER does not support the circuit '%1'").arg(name)));
    }

    QSet<QString> usedJacks;

    int row=0;
    for (auto ja: jackAssignments) {
        if (ja->isDisabled()) {
            row++;
            continue; // not interested in problems of disabled jacks
        }

        QString name = ja->jackName();
        if (usedJacks.contains(name)) {
            allProblems.append(
                new PatchProblem(row, 0, tr("Duplicate definition of parameter '%1'").arg(name)));
        }
        else { // check other errors only if not duplicate (avoid double errors)
            usedJacks.insert(name);
            for (auto problem: ja->collectProblems(patch)) {
                problem->setRow(row);
                allProblems.append(problem);
            }
        }
        row++;
    }
    return allProblems;
}
bool Circuit::jackIsArray(int row) const
{
    return the_firmware->jackIsArray(name, jackAssignments[row]->jackName());
}
QStringList Circuit::missingJacks(jacktype_t jackType) const
{
    QStringList jacks = jackType == JACKTYPE_INPUT
            ? the_firmware->inputsOfCircuit(name, JACKSELECTION_FULL)
            : the_firmware->outputsOfCircuit(name, JACKSELECTION_FULL);
    for (auto ja: jackAssignments)
        jacks.removeAll(ja->jackName());
    return jacks;
}
bool Circuit::hasMissingJacks() const
{
    return missingJacks(JACKTYPE_INPUT).count() > 0
            || missingJacks(JACKTYPE_OUTPUT).count() > 0;
}
bool Circuit::hasLEDMismatch()
{
    return checkLEDMismatches(false);
}
void Circuit::fixLEDMismatches()
{
   checkLEDMismatches(true);
}
bool Circuit::checkLEDMismatches(bool fixit)
{
    // LED mismatch:
    // For every buttonX which is assigned to a B...
    // there must be a ledX which is assigned to the corresponding L...

    // Phase 1: add missing LED assignments
    QList<JackAssignment *> toBeAdded;
    for (auto ja: jackAssignments) {
        if (ja->isInput() && ( // [encoder] has a button output
               ja->jackPrefix() == "button" ||
               ja->jackName() == "buttonup" ||
               ja->jackName() == "buttondown"))
        {
            Atom *atom = ja->atomAt(1);
            if (!atom || !atom->isRegister())
                continue;
            AtomRegister reg = *(AtomRegister *)atom;
            if (reg.getRegisterType() != REGISTER_BUTTON)
                continue; // Weird special thing. Ignore
            unsigned controller = reg.getController();
            unsigned number = reg.getNumber();

            // Conserve the "up" and "down" in "buttonup" and "buttondown"
            QString suffix = ja->jackName().mid(6);
            QString ledname = "led" + suffix;

            JackAssignment *ledja = findJack(ledname);
            if (!ledja) { // matching LED assignment not found
                if (fixit) {
                    JackAssignmentOutput *newJa = new JackAssignmentOutput(ledname);
                    newJa->replaceAtom(1, new AtomRegister(REGISTER_LED, controller, 0, number));
                    toBeAdded.append(newJa);
                    continue;
                }
                else
                    return true; // found mismatch
            }
            else {
                // parameter led... found, but is it set with a register?
                const Atom *ledAtom = ledja->atomAt(1);
                if (!ledAtom || !ledAtom->isRegister()) { // not present or no register
                    if (fixit) {
                        ledja->replaceAtom(1, new AtomRegister(REGISTER_LED, controller, 0, number));
                        continue;
                    }
                    else
                        return true;
                }
                else {
                    // parameter present and with a register. Now check if it's the correct LX.Y
                    AtomRegister ledReg = *(AtomRegister *)ledAtom;
                    if (ledReg.getRegisterType() != REGISTER_LED
                            || ledReg.getController() != controller
                            || ledReg.getNumber() != number)
                    {
                        if (fixit)
                            ledja->replaceAtom(1, new AtomRegister(REGISTER_LED, controller, 0, number));
                        else
                            return true;
                    }
                }
            }
        }
    }
    for (auto ja: toBeAdded)
        addJackAssignment(ja);

    // Phase 2: remove exceeding LED assignments
    QList<unsigned> toBeRemoved; // indices of to assignment to be removed
    unsigned nr = 0;
    for (auto ja: jackAssignments) {
        if (ja->isOutput() && ja->jackPrefix() == "led") { // [encoder] has a LED input
            QString suffix = ja->jackName().mid(3);
            if (!findJack("button" + suffix)) {
                if (fixit)
                    toBeRemoved.append(nr);
                else
                    return true;
            }
        }
        nr++;
    }

    // remove in reverse direction (avoids index chaos)
    while (!toBeRemoved.isEmpty())
       deleteJackAssignment(toBeRemoved.takeLast());

    return false;
}
void Circuit::changeCircuit(QString newCircuit)
{
    name = newCircuit;
    QList<JackAssignment *> newJacks;
    for (auto ja: jackAssignments) {
        QString asString = ja->toString();
        delete ja;
        JackAssignment *newJa = JackAssignment::parseJackLine(newCircuit, asString);
        newJacks.append(newJa);
    }
    jackAssignments.clear();
    jackAssignments = newJacks;
}
void Circuit::collectRegisterAtoms(RegisterList &sl, bool skipOverlayedControls) const
{
    bool skipControls = skipOverlayedControls && usesSelect();
    for (auto ja: jackAssignments) {
        if (!ja->isDisabled())
            ja->collectRegisterAtoms(sl, skipControls);
    }
}
void Circuit::removeRegisterReferences(RegisterList &rl)
{
    for (auto ja: jackAssignments)
        ja->removeRegisterReferences(rl);

}
QString Circuit::toString() const
{
    QString s;
    for (qsizetype i=0; i<comment.size(); i++)
        if (comment[i].isEmpty())
             s += "#\n";
         else
             s += "# " + comment[i] + "\n";

    if (disabled)
        s += "# ";
    s += "[" + name + "]\n";

    for (qsizetype i=0; i<jackAssignments.length(); i++)
    {
        QString jackLine = jackAssignments[i]->toString() + "\n";
        s += jackLine;
    }
    s += "\n";
    return s;
}
QString Circuit::toCleanString() const
{
    QString s = "[" + name + "]\n";
    for (qsizetype i=0; i<jackAssignments.length(); i++)
    {
        QString jackLine = jackAssignments[i]->toBare();
        if (jackLine != "") {
            jackLine.replace("=", " = ");
            jackLine.replace("*", " * ");
            jackLine.replace("+", " + ");
            s += "    " + jackLine + "\n";
        }
    }
    return s;
}
QString Circuit::toBare() const
{
    if (disabled)
        return "";
    QString s = "[" + name + "]\n";
    for (qsizetype i=0; i<jackAssignments.length(); i++)
    {
        QString jackLine = jackAssignments[i]->toBare() + "\n";
        s += jackLine;
    }
    return s;
}
QString Circuit::getComment() const
{
    if (comment.empty())
        return "";
    else
        return comment.join('\n') + "\n";
}
QString Circuit::getCommentWithHashes() const
{
    if (comment.empty())
        return "";
    QString text = "";
    for (auto& line: comment) {
        text += "# " + line + "\n";
    }
    return text;
}
void Circuit::setDisabledWithJacks(bool d)
{
    disabled = d;
    for (auto ja: jackAssignments)
        ja->setDisabled(d);
}
void Circuit::setComment(QString c)
{
    comment = c.trimmed().split("\n");
}
void Circuit::removeComment()
{
    comment.clear();
    if (haveBookmark && bmRow == ROW_COMMENT)
        bmRow = ROW_CIRCUIT;
}
