#include "undohistory.h"
#include "QtCore/qdebug.h"
#include "tuning.h"

UndoHistory::UndoHistory()
    : redoPointer(0)
{
}


UndoHistory::~UndoHistory()
{
    clear();
}


void UndoHistory::clear()
{
    for (qsizetype i=0; i<steps.size(); i++)
        delete steps[i];
    steps.clear();
    redoPointer = 0;
}


void UndoHistory::snapshot(QString name, const Patch *patch)
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
