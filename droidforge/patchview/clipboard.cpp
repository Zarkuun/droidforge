#include "clipboard.h"
#include "patch.h"

Clipboard *the_clipboard = 0;

Clipboard::Clipboard()
{
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

void Clipboard::copyFromSelection(Selection *sel, const PatchSection *section)
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
