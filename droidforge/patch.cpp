#include "patch.h"

Patch::Patch()
    : version(0)
{
}


QString Patch::toString()
{
    QString s;
    if (!title.isEmpty())
        s += "# " + title + "\n";

    if (version > 0 || !libraryId.isEmpty())
        s += "# LIBRARY: id=" + libraryId + ", version=" + QString::number(version) + "\n";

    if (!description.isEmpty()) {
        if (!s.isEmpty())
            s += "\n";
        for (qsizetype i=0; i<description.length(); i++) {
            s += "# " + description[i] + "\n";
        }
        s += "\n";
    }

    for (qsizetype i=0; i<controllers.length(); i++)
        s += "[" + controllers[i] + "]\n";

    if (!s.isEmpty())
        s += "\n";

    for (qsizetype i=0; i<sections.length(); i++)
        s += sections[i].toString();

    while (s.endsWith("\n\n"))
        s.chop(1);
    return s;

}
