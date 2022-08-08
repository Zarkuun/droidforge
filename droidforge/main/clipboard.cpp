#include "clipboard.h"
#include "globals.h"
#include "parseexception.h"
#include "patch.h"
#include "patchparser.h"

#include <QGuiApplication>
#include <QClipboard>

Clipboard *the_clipboard = 0;

Clipboard::Clipboard()
    : ignoreNextGlobalClipboardChange(false)
{
    // Hm. This seems like a hack. The first clipboard goes global.
    // Intermediate clipboards come later and will not overwrite this.
    if (the_clipboard == 0)
        the_clipboard = this;
}

Clipboard::Clipboard(const QList<Circuit *>cs)
{
    for (auto circuit: cs)
        circuits.append(circuit->clone());
}

Clipboard::~Clipboard()
{
    clear();
}

void Clipboard::copyFromSelection(const Selection *sel, const PatchSection *section)
{
    const CursorPosition &from = sel->fromPos();
    const CursorPosition &to = sel->toPos();

    clear();
    if (sel->isCircuitSelection())
        for (int i=from.circuitNr; i<=to.circuitNr; i++) {
            circuits.append(section->circuit(i)->clone());
        }
    else {
        const Circuit *circuit = section->circuit(from.circuitNr);

        if (sel->isJackSelection())
            for (int i=from.row; i<=to.row; i++)
                jackAssignments.append(circuit->jackAssignment(i)->clone());

        else if (sel->isAtomSelection()) {
            for (int i=from.column; i<=to.column; i++) {
                const Atom *atom = circuit->jackAssignment(from.row)->atomAt(i);
                atoms.append(atom ? atom->clone() : 0);
            }
        }
        else // circuit comment selection
            comment = circuit->getComment();
    }

    ignoreNextGlobalClipboardChange = true;
}

bool Clipboard::isEmpty() const
{
    return circuits.isEmpty()
            && jackAssignments.isEmpty()
            && atoms.isEmpty()
            && comment.isEmpty();
}

unsigned Clipboard::numCircuits() const
{
    return circuits.size();
}

bool Clipboard::isComment() const
{
    return !comment.isEmpty();
}

Patch *Clipboard::getAsPatch() const
{
    Patch *patch = new Patch();
    PatchSection *ps = new PatchSection();
    for (auto circuit: circuits)
        ps->addCircuit(circuit->clone());
    patch->addSection(ps);
    return patch;
}

void Clipboard::copyToGlobalClipboard() const
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(toString());

}

void Clipboard::copyFromGlobalClipboard()
{
    if (ignoreNextGlobalClipboardChange) {
        ignoreNextGlobalClipboardChange = false;
        return;
    }
    clear();
    try {
        Patch patch;
        PatchParser parser;
        QClipboard *clipboard = QGuiApplication::clipboard();
        parser.parseString(clipboard->text(), &patch);
        for (auto section: patch.getSections())
            for (auto circuit: section->getCircuits())
                circuits.append(circuit->clone());
    }
    catch (ParseException &e) {
    }

}

QString Clipboard::toString() const
{
    QString text;
    for (auto circuit: circuits)
        text += circuit->toString();
    if (text != "")
        return text;

    for (auto ja: jackAssignments) {
        text += ja->toString();
        text += "\n";
    }
    if (text != "")
        return text;

    for (auto atom: atoms)
        if (atom)
            text += atom->toString();
    if (text != "")
        return text;

    if (comment != "")
        return comment;

    else
        return "";
}

unsigned Clipboard::numJacks() const
{
    return jackAssignments.size();
}

unsigned Clipboard::numAtoms() const
{
    return atoms.size();
}

void Clipboard::clear()
{
    for (auto circuit: circuits)
        delete circuit;
    circuits.clear();

    for (auto ja: jackAssignments)
        delete ja;
    jackAssignments.clear();

    for (auto atom: atoms)
        delete atom;
    atoms.clear();

    comment.clear();
}
