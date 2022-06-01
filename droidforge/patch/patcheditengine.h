#ifndef PATCHEDITENGINE_H
#define PATCHEDITENGINE_H

#include "patch.h"
#include "patchversion.h"

#include <QList>

/* Combines all the state of the editing of *one* patch. In case
 * of multiple patch windows, there is one such object per window.
 * The Clipboard is thus *not* part of this class. */

class PatchEditEngine : public Patch
{
    QList<PatchVersion *> versions;
    int redoPointer;
    int versionOnDisk;

    bool patching;
    unsigned patchingStartSection; // invalid if patching == false
    CursorPosition patchingStartPosition; // invalid if patching == false

public:
    PatchEditEngine(const Patch *patch);
    PatchEditEngine(); // empty
    ~PatchEditEngine();
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

    bool isPatching() const { return patching; };
    void startPatching();
    void stopPatching();

private:
    void clearVersions();
};

#endif // PATCHEDITENGINE_H
