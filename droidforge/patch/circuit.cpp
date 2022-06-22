#include "circuit.h"
#include "droidfirmware.h"
#include "globals.h"
#include "jackassignmentoutput.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

Circuit::Circuit(QString name, const QStringList &comment, bool disabled)
    : name(name)
    , comment(comment)
    , disabled(disabled)
    , folded(false)
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

}
void Circuit::insertJackAssignment(JackAssignment *ja, int index)
{
    jackAssignments.insert(index, ja);
}
JackAssignment *Circuit::findJack(const QString name)
{
    for (auto ja: jackAssignments)
        if (ja->jackName() == name)
            return ja;
    return 0;
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
            i--;
        }
    }
}
QString Circuit::nextJackArrayName(const QString &jackName, bool isInput)
{
    QString prefix = jackName;
    while (prefix != "" && prefix.back().isDigit())
        prefix.chop(1);;
    unsigned size = the_firmware->jackArraySize(name, prefix, isInput);
    if (size == 0)
        return "";

    QString next;
    for (unsigned i=1; i<=size; i++) {
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
QList<PatchProblem *> Circuit::collectProblems(const Patch *patch) const
{
    QList<PatchProblem *> allProblems;
    // TODO: Account for RAM usage in DROID
    if (!the_firmware->circuitExists(name)) {
        allProblems.append(
            new PatchProblem(-2, 0, tr("There is no such circuit with the name '%1'").arg(name)));
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
    for (auto ja: jackAssignments) {
        if (ja->jackPrefix() == "button" ||
                ja->jackName() == "buttonup" ||
                ja->jackName() == "buttondown")
        {
            Atom *atom = ja->atomAt(1);
            if (!atom || !atom->isRegister())
                continue;
            AtomRegister reg = *(AtomRegister *)atom;
            if (reg.getRegisterType() != REGISTER_BUTTON)
                continue; // Weird special thing. Ignore
            unsigned controller = reg.controller();
            unsigned number = reg.number();

            QString suffix = ja->jackName().mid(6);
            QString ledname = "led" + suffix;

            JackAssignment *ledja = findJack(ledname);
            if (!ledja) {
                if (fixit) {
                    JackAssignmentOutput *newJa = new JackAssignmentOutput(ledname);
                    newJa->replaceAtom(1, new AtomRegister(REGISTER_LED, controller, number));
                    this->addJackAssignment(newJa);
                    continue;
                }
                else
                    return true;
            }

            const Atom *ledAtom = ledja->atomAt(1);
            if (!ledAtom || !ledAtom->isRegister()) {
                if (fixit) {
                    ledja->replaceAtom(1, new AtomRegister(REGISTER_LED, controller, number));
                    continue;
                }
                else
                    return true;
            }

            AtomRegister ledReg = *(AtomRegister *)ledAtom;
            if (ledReg.getRegisterType() != REGISTER_LED
                || ledReg.controller() != controller
                || ledReg.number() != number)
            {
                if (fixit)
                    ledja->replaceAtom(1, new AtomRegister(REGISTER_LED, controller, number));
                else
                    return true;
            }
        }
    }

    // Phase 2: remove exceeding LED assignments
    QList<unsigned> toRemove;
    unsigned nr = 0;
    for (auto ja: jackAssignments) {
        if (ja->jackPrefix() == "led") {
            QString suffix = ja->jackName().mid(3);
            if (!findJack("button" + suffix)) {
                if (fixit)
                    toRemove.append(nr);
                else
                    return true;
            }
        }
        nr++;
    }

    // remove in reverse direction (avoids index chaos)
    while (!toRemove.isEmpty())
       deleteJackAssignment(toRemove.takeLast());

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
bool Circuit::needG8() const
{
    // TODO: Toplevel lösen mit atom-iterator
    for (qsizetype i=0; i<jackAssignments.length(); i++) {
        if (jackAssignments[i]->needG8())
            return true;
    }
    return false;
}
bool Circuit::needX7() const
{
    // TODO: This seems somewhat hacky...
    if (name.startsWith("midi") || name == "firefacecontrol")
        return true;
    for (qsizetype i=0; i<jackAssignments.length(); i++) {
        if (jackAssignments[i]->needX7())
            return true;
    }
    return false;
}
void Circuit::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (auto ja: jackAssignments)
        ja->swapControllerNumbers(fromNumber, toNumber);
}
void Circuit::shiftControllerNumbers(int number, int by)
{
    for (auto ja: jackAssignments)
        ja->shiftControllerNumbers(number, by);
}
void Circuit::collectRegisterAtoms(RegisterList &sl) const
{
    for (auto ja: jackAssignments) {
        if (!ja->isDisabled())
            ja->collectRegisterAtoms(sl);
    }
}
void Circuit::remapRegister(AtomRegister from, AtomRegister to)
{
    for (auto ja: jackAssignments)
        ja->remapRegister(from, to);
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
             s += "\n";
         else
             s += "# " + comment[i] + "\n"; // TODO Multiline

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
}
