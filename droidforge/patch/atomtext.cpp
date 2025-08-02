#include "atomtext.h"
#include "globals.h"

AtomText *AtomText::clone() const
{
    return new AtomText(text);
}

QString AtomText::toString() const
{
    return QString("\"") + text + "\"";
}

QString AtomText::problemAsInput(const Patch *) const
{
    return "";
}

QString AtomText::problemAsOutput(const Patch *) const
{
    return TR("You cannot use a text as an output parameter");
}


