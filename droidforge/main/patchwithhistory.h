#ifndef PATCHWITHHISTORY_H
#define PATCHWITHHISTORY_H

#include "patch.h"
#include "editorstate.h"

#include <QList>

class PatchWithHistory
{
    QList<EditorState *> steps;
    unsigned redoPointer;
    unsigned versionOnDisk;

public:
    PatchWithHistory();
    ~PatchWithHistory();
    void reset(const Patch *patch);
    bool isModified() const;
    void clearModified();
    void snapshot(const Patch *patch, QString name="");
    qsizetype size() const { return steps.size(); };
    Patch *undo();
    Patch *redo();
    QString nextTitle() const;
    bool undoPossible() const;
    bool redoPossible() const;
    QString nextUndoTitle() const;
    QString nextRedoTitle() const;

private:
    void clear();
};

#endif // PATCHWITHHISTORY_H
