#ifndef UNDOHISTORY_H
#define UNDOHISTORY_H

#include "patch.h"
#include "editorstate.h"

#include <QList>

class UndoHistory
{
    QList<EditorState *> steps;
    unsigned redoPointer;

public:
    UndoHistory();
    ~UndoHistory();
    void clear();
    void snapshot(QString name, const Patch *patch);
    qsizetype size() const { return steps.size(); };
    Patch *undo();
    Patch *redo();
    QString nextTitle() const;
    bool undoPossible() const;
    bool redoPossible() const;
    QString nextUndoTitle() const;
    QString nextRedoTitle() const;
};

#endif // UNDOHISTORY_H
