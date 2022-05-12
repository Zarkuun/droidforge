#include "undohistory.h"
#include "tuning.h"

UndoHistory::UndoHistory()
    : redoPointer(0)
    , versionOnDisk(0)
{
}

UndoHistory::~UndoHistory()
{
    clear();
}

void UndoHistory::reset(const Patch *patch)
{
    clear();
    snapshot(patch);
    versionOnDisk = redoPointer;
}

bool UndoHistory::isModified() const
{
    return versionOnDisk != redoPointer;
}

void UndoHistory::clearModified()
{
    versionOnDisk = redoPointer;
}

void UndoHistory::clear()
{
    for (qsizetype i=0; i<steps.size(); i++)
        delete steps[i];
    steps.clear();
    redoPointer = 0;
}

void UndoHistory::snapshot(const Patch *patch, QString name)
{
    // One new edit step erases all possible redos
    while (redoPointer < steps.size()) {
        delete steps[steps.size()-1];
        steps.removeLast();
    }

    steps.append(new EditorState(name, patch));
    redoPointer++; // is always at the end now

    while (steps.size() > UNDO_HISTORY_SIZE) {
        delete steps[0];
        steps.removeFirst();
        redoPointer--;
    }
}

Patch *UndoHistory::undo()
{
    // assume undoPossible()
    return steps[--redoPointer]->getPatch()->clone();
}

Patch *UndoHistory::redo()
{
    // assume redoPossible()
    return steps[redoPointer++]->getPatch()->clone();
}

QString UndoHistory::nextTitle() const
{
    return steps.last()->getName();
}

bool UndoHistory::undoPossible() const
{
    return redoPointer > 0;
}

bool UndoHistory::redoPossible() const
{
    return redoPointer < steps.size();
}

QString UndoHistory::nextUndoTitle() const
{
    return steps[redoPointer-1]->getName();
}

QString UndoHistory::nextRedoTitle() const
{
    return steps[redoPointer]->getName();
}
