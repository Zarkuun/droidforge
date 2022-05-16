#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"
#include "registercomments.h"

#include <QStringList>

class Patch
{
    QString title;
    QStringList description;
    QString libraryMetaData; // break into structure later
    RegisterComments *registerComments;
    QStringList controllers;
    QList<PatchSection *> sections;
    qsizetype sectionIndex; // part of cursor position

public:
    Patch();
    ~Patch();
    Patch *clone() const;
    qsizetype numControllers() const { return controllers.size(); };
    QString controller(qsizetype i) const { return controllers[i]; };
    qsizetype numSections() const { return sections.size(); };
    qsizetype currentSectionIndex() const { return sectionIndex; };
    void setCurrentSectionIndex(qsizetype i) { sectionIndex = i; };
    PatchSection *section(qsizetype i) { return sections[i]; };
    void addSection(PatchSection *section);
    void insertSection(int index, PatchSection *section);
    void deleteSection(int index);
    void reorderSections(int fromindex, int toindex);
    void addController(QString name) { controllers.append(name); };
    bool saveToFile(QString filename);
    void addDescriptionLine(const QString &line);
    void setDescription(const QString &d);
    void addRegisterComment(
            QChar registerName,
            unsigned controller,
            unsigned number,
            const QString &shorthand,
            const QString &atomcomment);


    QString toString();
    const QString &getTitle() const;
    QString getDescription() const;
    void setTitle(const QString &newTitle);
    const QString &getLibraryMetaData() const { return libraryMetaData; }
    void setLibraryMetaData(const QString &newLibraryMetaData) { libraryMetaData = newLibraryMetaData; }
    QStringList allCables() const;
    bool needG8() const;
    bool needX7() const;
};

#endif // PATCH_H
