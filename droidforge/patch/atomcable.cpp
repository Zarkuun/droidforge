#include "atomcable.h"
#include "globals.h"
#include "patchproblem.h"
#include "patch.h"

AtomCable *AtomCable::clone() const
{
    return new AtomCable(name);
}
QString AtomCable::toString() const
{
    return QString("_") + name;
}
QString AtomCable::problemAsInput(const Patch *patch) const
{
    int numAsOutput = 0;
    int numAsInput = 0;
    patch->findCableConnections(name, numAsInput, numAsOutput);
    if (numAsOutput == 0)
        return TR("There is no output connected to this cable"); else if (numAsInput == 0)
        return TR("There is no input connected to this cable");
    else if (numAsOutput > 1)
        return TR("There is more than one output connected to this cable");
    else
        return "";
}
QString AtomCable::problemAsOutput(const Patch *patch) const
{
    return problemAsInput(patch);
}
QString AtomCable::nextCableName(const QString &name)
{
    // HIRN -> HIRN2
    if (!name.back().isDigit())
        return name + "2";

    // HIRN1 -> HIRN2
    // HIRN17 -> HIRN18
    int i = name.length();
    while (name[i-1].isDigit())
        i--;

    // i now points to first digit of the end part
    QString stem = name.mid(0, i);
    int number = name.mid(i).toInt();
    return stem + QString::number(number+1);
}
void AtomCable::rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode)
{
    switch (mode) {
    case RewriteCablesDialog::SEARCH:
        name = name.toUpper().replace(remove.toUpper(), insert.toUpper());
        break;

    case RewriteCablesDialog::PREFIX:
        name = insert + name;
        break;

    case RewriteCablesDialog::SUFFIX:
        name = name + insert;
        break;
    }
}

void AtomCable::incrementForExpansion(const Patch *)
{
    // Idea: _FOO_BAR_2 --> _FOO_FOO_BAR_3.
    // Cables without a number suffix stay as they are

    if (!name.back().isDigit())
        return;

    name = nextCableName(name);
}
