#include "clipboard.h"

Clipboard::Clipboard()
{
}

Clipboard::~Clipboard()
{
    clear();
}

void Clipboard::copyFromSelection(Selection *sel, PatchSection *section)
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

bool Clipboard::isCircuits() const
{
    return !circuits.isEmpty();
}

bool Clipboard::isComment() const
{
    return !comment.isEmpty();
}

bool Clipboard::isJacks() const
{
    return !jackAssignments.isEmpty();
}

bool Clipboard::isAtoms() const
{
    return !atoms.isEmpty();
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
