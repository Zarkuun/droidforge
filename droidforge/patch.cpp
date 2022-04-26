#include "patch.h"

#include <QFile>
#include <QTextStream>





Patch::Patch()
    : registerComments(new RegisterComments())
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
    return newpatch;
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
