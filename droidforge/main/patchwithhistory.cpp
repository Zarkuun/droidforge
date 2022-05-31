#include "patchwithhistory.h"
#include "tuning.h"

PatchWithHistory::PatchWithHistory()
    : redoPointer(0)
    , versionOnDisk(0)
{
}

PatchWithHistory::~PatchWithHistory()
{
    clear();
}

void PatchWithHistory::reset(const Patch *patch)
{
    clear();
    snapshot(patch);
    versionOnDisk = redoPointer;
}

bool PatchWithHistory::isModified() const
{
    return versionOnDisk != redoPointer;
}

void PatchWithHistory::clearModified()
{
    versionOnDisk = redoPointer;
}

void PatchWithHistory::clear()
{
    for (qsizetype i=0; i<steps.size(); i++)
        delete steps[i];
    steps.clear();
    redoPointer = 0;
}

void PatchWithHistory::snapshot(const Patch *patch, QString name)
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

Patch *PatchWithHistory::undo()
{
    // assume undoPossible()
    return steps[--redoPointer]->getPatch()->clone();
}

Patch *PatchWithHistory::redo()
{
    // assume redoPossible()
    return steps[redoPointer++]->getPatch()->clone();
}

QString PatchWithHistory::nextTitle() const
{
    return steps.last()->getName();
}

bool PatchWithHistory::undoPossible() const
{
    return redoPointer > 0;
}

bool PatchWithHistory::redoPossible() const
{
    return redoPointer < steps.size();
}

QString PatchWithHistory::nextUndoTitle() const
{
    return steps[redoPointer-1]->getName();
}

QString PatchWithHistory::nextRedoTitle() const
{
    return steps[redoPointer]->getName();
}
