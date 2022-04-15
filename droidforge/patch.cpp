#include "patch.h"

Patch::Patch()
    : version(0)
{
}


Patch::~Patch()
{
    for (qsizetype i=0; i<sections.length(); i++)
        delete sections[i];
}


Patch *Patch::clone() const
{
    Patch *newpatch = new Patch();
    newpatch->title = title;
    newpatch->description = description;
    newpatch->libraryId = libraryId;
    newpatch->version = version;
    newpatch->controllers = controllers;

    for (unsigned i=0; i<sections.size(); i++) {
        newpatch->sections.append(sections[i]->clone());
    }
    return newpatch;
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
        s += sections[i]->toString();

    while (s.endsWith("\n\n"))
        s.chop(1);
    return s;

}
