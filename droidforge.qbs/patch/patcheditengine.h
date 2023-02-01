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
    QString filePath;

    bool patching;
    unsigned patchingStartSection; // invalid if patching == false
    CursorPosition patchingStartPosition; // invalid if patching == false

public:
    PatchEditEngine(); // empty
    ~PatchEditEngine();
    const QString &getFilePath() const { return filePath; };
    bool hasFilename() const { return filePath != ""; };
    void setFilePath(const QString &f) { filePath = f; };
    void startFromScratch();
    bool isModified() const;
    void clearModified();
    bool save(QString filename);
    void commit(QString message="");
    qsizetype size() const { return versions.size(); };
    void undo();
    void redo();
    QString lastCommitTitle() const;
    bool undoPossible() const;
    bool redoPossible() const;
    QString nextUndoTitle() const;
    QString nextRedoTitle() const;
    void commitCursorPosition();
    void commitFolding();

    bool isPatching() const { return patching; };
    bool isPatchingFrom(const CursorPosition &pos) const;
    void startPatching();
    void stopPatching();
    PatchSection *getPatchingStartSection();
    const CursorPosition &getPatchingStartPosition() { return patchingStartPosition; };

private:
    void clearVersions();
};

#endif // PATCHEDITENGINE_H
