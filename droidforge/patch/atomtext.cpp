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

QString AtomText::problemAsOutput(const Patch *) const
{
    return TR("You cannot use a text for an output parameter");

}
