#include "jackassignment.h"

QString JackAssignment::toString()
{
    QString s;
    if (disabled)
        s += "# ";
    s = "    " + jack + " = " + value;
    if (!comment.isEmpty())
        s += " # " + comment;

    if (jackType == JACKTYPE_INPUT)
        s += "<INPUT>";
    else if (jackType == JACKTYPE_OUTPUT)
        s += "<OUTPUT>";
    else
        s += "???";

    return s;
}
