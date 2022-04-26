#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"
#include "registercomments.h"

#include <QStringList>

class Patch
{
    QString title;
    QStringList description;
    QString libraryId;
    unsigned version;
    RegisterComments *registerComments;
    QStringList controllers;


public:
    Patch();
    ~Patch();
    Patch *clone() const;
    qsizetype numControllers() const { return controllers.size(); };
    QString controller(qsizetype i) const { return controllers[i]; };
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

    QList<PatchSection *> sections;

    QString toString();
    const QString &getTitle() const;
    QString getDescription() const;
    void setTitle(const QString &newTitle);
};

#endif // PATCH_H
