#ifndef VERSIONEDPATCH_H
#define VERSIONEDPATCH_H

#include "patch.h"
#include "patchversion.h"

#include <QList>

class VersionedPatch : public Patch
{
    QList<PatchVersion *> versions;
    int redoPointer;
    int versionOnDisk;


public:
    VersionedPatch(const Patch *patch);
    VersionedPatch(); // empty
    ~VersionedPatch();
    void startFromScratch();
    bool isModified() const;
    bool saveToFile(QString filename);
    void commit(QString message="");
    qsizetype size() const { return versions.size(); };
    void undo();
    void redo();
    QString lastCommitTitle() const;
    bool undoPossible() const;
    bool redoPossible() const;
    QString nextUndoTitle() const;
    QString nextRedoTitle() const;

private:
    void clearVersions();
};

#endif // VERSIONEDPATCH_H
