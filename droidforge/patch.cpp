#include "patch.h"

#include <QFile>
#include <QTextStream>

// TODO: use index and number correctly
// Controller number goes from 1 ... 16
// Indices start at 0

Patch::Patch()
    : registerComments(new RegisterComments())
    , sectionIndex(0)
{
}


Patch::~Patch()
{
    delete registerComments;
    for (qsizetype i=0; i<sections.length(); i++)
        delete sections[i];
}


Patch *Patch::clone() const
{
    Patch *newpatch = new Patch();
    newpatch->title = title;
    newpatch->description = description;
    newpatch->libraryMetaData = libraryMetaData;
    delete newpatch->registerComments;
    newpatch->registerComments = registerComments->clone();
    newpatch->controllers = controllers;
    for (unsigned i=0; i<sections.size(); i++) {
        newpatch->sections.append(sections[i]->clone());
    }
    newpatch->sectionIndex = sectionIndex;
    return newpatch;
}

void Patch::addSection(PatchSection *section)
{
    sections.append(section);
}

void Patch::insertSection(int index, PatchSection *section)
{
    sections.insert(index, section);
}

void Patch::deleteSection(int index)
{
    PatchSection *s = sections[index];
    sections.remove(index);
    delete s;
}

void Patch::reorderSections(int fromindex, int toindex)
{
    PatchSection *moved = sections[fromindex];
    sections.remove(fromindex);
    sections.insert(toindex, moved);
}

void Patch::swapControllersSmart(int fromindex, int toindex)
{
    QStringList newControllers;
    for (qsizetype i=0; i<controllers.size(); i++) {
        if (i == fromindex)
            newControllers.append(controllers[toindex]);
        else if (i == toindex)
            newControllers.append(controllers[fromindex]);
        else
            newControllers.append(controllers[i]);
    }
    controllers = newControllers;
    swapControllerNumbers(fromindex+1, toindex+1);
}

void Patch::removeController(int index)
{
    for (auto section: sections)
        section->shiftControllerNumbers(index + 1);
    controllers.remove(index);
}


bool Patch::saveToFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
          return false;
    QTextStream stream(&file);
    stream << toString();
    stream.flush();
    file.close();
    return stream.status() == QTextStream::Ok;
}

void Patch::addDescriptionLine(const QString &line)
{
    description.append(line);
}

void Patch::setDescription(const QString &d)
{
    description = d.split('\n');
    if (d.endsWith("\n"))
        description.removeLast();
}

void Patch::addRegisterComment(QChar registerName, unsigned controller, unsigned number, const QString &shorthand, const QString &comment)
{
    registerComments->addComment(
                new AtomRegister(registerName, controller, number),
                shorthand, comment);
}

const QString &Patch::getTitle() const
{
    return title;
}

QString Patch::getDescription() const
{
    if (description.empty())
        return "";
    else
        return description.join("\n") + "\n";
}

void Patch::setTitle(const QString &newTitle)
{
    title = newTitle;
}


QStringList Patch::allCables() const
{
    QStringList cables;
    for (qsizetype i=0; i<sections.length(); i++)
        sections[i]->collectCables(cables);
    cables.removeDuplicates();
    cables.sort();
    return cables;
}

bool Patch::needG8() const
{
    for (qsizetype i=0; i<sections.length(); i++)
        if (sections[i]->needG8())
            return true;

    return false;
}

bool Patch::needX7() const
{
    for (qsizetype i=0; i<sections.length(); i++)
        if (sections[i]->needX7())
            return true;

    return false;
}

void Patch::collectRegisterAtoms(RegisterList &sl) const
{
    for (auto section: sections)
        section->collectRegisterAtoms(sl);
}

void Patch::remapRegister(AtomRegister from, AtomRegister to)
{
    for (auto section: sections)
        section->remapRegister(from, to);
}

void Patch::removeRegisterReferences(RegisterList &rl, int ih, int oh)
{
    for (auto section: sections)
        section->removeRegisterReferences(rl, ih, oh);
}

void Patch::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (auto section: sections)
        section->swapControllerNumbers(fromNumber, toNumber);
}

QString Patch::toString()
{
    QString s;
    if (title.isEmpty())
        s += "# Untitled patch\n";
    else
        s += "# " + title + "\n";

    if (!libraryMetaData.isEmpty())
        s += "# LIBRARY: " + libraryMetaData;

    s += "\n";

    if (!description.isEmpty()) {
        if (!s.isEmpty())
            s += "\n";
        for (qsizetype i=0; i<description.length(); i++) {
            s += "# " + description[i] + "\n";
        }
        s += "\n";
    }

    s += registerComments->toString();

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
