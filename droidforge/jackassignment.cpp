#include "jackassignment.h"

QString JackAssignment::toString()
{
    QString s;
    if (disabled)
        s += "# ";
    s = "    " + jack + " = " + value;
    if (!comment.isEmpty())
        s += " # " + comment;
    return s;
}
