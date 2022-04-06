#include "circuit.h"

QString Circuit::toString()
{
    QString s;
    if (!comment.isEmpty()) {
        for (qsizetype i=0; i<comment.size(); i++)
             s += "# " + comment[i] + "\n"; // TODO Multiline
    }

    // TODO: disabled
    s += "[" + name + "]\n";

    for (qsizetype i=0; i<jackAssignments.length(); i++)
    {
        s += jackAssignments[i].toString() + "\n";
    }
    s += "\n";
    return s;
}
