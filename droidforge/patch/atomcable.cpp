#include "atomcable.h"
#include "globals.h"
#include "patch.h"

AtomCable *AtomCable::clone() const
{
    return new AtomCable(name);
}
QString AtomCable::toString() const
{
    return QString("_") + name;
}
bool AtomCable::isCable(const QString &n) const
{
    return name == n;
}
QString AtomCable::problemAsInput(const Patch *patch) const
{
    int numAsOutput = 0;
    int numAsInput = 0;
    patch->findCableConnections(name, numAsInput, numAsOutput);
    if (numAsOutput == 0)
        return TR("There is no output connected to this cable");
    else
        return "";
}
QString AtomCable::problemAsOutput(const Patch *patch) const
{
    int numAsOutput = 0;
    int numAsInput = 0;
    patch->findCableConnections(name, numAsInput, numAsOutput);
    if (numAsOutput > 1)
        return TR("There is more than one output connected to this cable");
    else if (numAsInput == 0)
        return TR("There is no input connected to this cable");
    else
        return "";
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
void AtomCable::rewriteCablePrefix(const QString &fromPrefix, const QString &toPrefix)
{
    if (name.startsWith(fromPrefix))
        name = toPrefix + name.mid(fromPrefix.length());
}
void AtomCable::incrementForExpansion(const Patch *)
{
    name = nextCableName(name);
}
QString AtomCable::nextCableName(const QString &name)
{
    // Split name into groups of digits and non-digits.
    // The use last group of digits and increase it by
    // one. Then recombine.

    QStringList groups;
    QString group;
    enum {
        DIGITS,
        OTHER,
        NONE,
    } currentType = NONE, newType;
    int lastDigitGroup = -1;

    for (int i=0; i<name.size(); i++) {
        QChar c = name[i];
        if (c.isDigit())
            newType = DIGITS;
        else
            newType = OTHER;
        if (newType != currentType) {
            if (group.size())
                groups.append(group);
            if (newType == DIGITS)
                lastDigitGroup = groups.size();
            group = "";
        }
        currentType = newType;
        group.append(c);
    }
    if (group.size())
        groups.append(group);

    QString newName = "";
    for (int g=0; g<groups.size(); g++) {
        if (g == lastDigitGroup) {
            QString digitgroup = groups[lastDigitGroup];
            int number = digitgroup.toInt() + 1;
            newName += QString::number(number);
        }
        else
            newName += groups[g];
    }
    return newName;
}
