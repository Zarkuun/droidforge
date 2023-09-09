#include "patcheditengine.h"
#include "tuning.h"
#include "globals.h"

#include <QTextStream>
#include <QFile>

// Does *not* do a commit()!
PatchEditEngine::PatchEditEngine()
    : redoPointer(-1)
    , versionOnDisk(-1)
    , patching(false)
{
    addSection(new PatchSection()); // there always must be one section
}

PatchEditEngine::~PatchEditEngine()
{
    clearVersions();
}
void PatchEditEngine::clearVersions()
{
    for (auto version: versions)
        delete version;
    versions.clear();
}
void PatchEditEngine::startFromScratch()
{
    patching = false;
    redoPointer = -1;
    versionOnDisk = -2;
    clear();
    clearVersions();
    filePath = "";
}
bool PatchEditEngine::isModified() const
{
    return versionOnDisk != redoPointer;
}
void PatchEditEngine::clearModified()
{
    versionOnDisk = redoPointer;
}
bool PatchEditEngine::save(QString filePath)
{
    if (saveToFile(filePath)) {
        versionOnDisk = redoPointer;
        setFilePath(filePath);
        return true;
    }
    else
        return false;
}
void PatchEditEngine::commit(QString message)
{
    // One new edit step erases all possible redos
    while (redoPointer + 1 < versions.size()) {
        delete versions.last();
        versions.removeLast();
    }

    versions.append(new PatchVersion(message, this)); // is cloned here
    redoPointer++; // is always at the end now
    if (versionOnDisk == -2)
        versionOnDisk = redoPointer; // initial commit

    while (versions.size() > UNDO_HISTORY_SIZE) {
        delete versions[0];
        versions.removeFirst();
        redoPointer--;
    }

    updateProblems(); // This is the one and only place where we do this!
}
void PatchEditEngine::undo()
{
    Q_ASSERT(undoPossible());
    versions[--redoPointer]->getPatch()->cloneInto(this);
    updateProblems(); // This is the one and only place where we do this!
}
void PatchEditEngine::redo()
{
    Q_ASSERT(redoPossible());
    versions[++redoPointer]->getPatch()->cloneInto(this);
    updateProblems(); // This is the one and only place where we do this!
}
bool PatchEditEngine::undoPossible() const
{
    return redoPointer > 0;
}
bool PatchEditEngine::redoPossible() const
{
    return redoPointer + 1 < versions.size();
}
QString PatchEditEngine::nextUndoTitle() const
{
    return versions[redoPointer]->getName();
}
QString PatchEditEngine::nextRedoTitle() const
{
    return versions[redoPointer+1]->getName();
}
void PatchEditEngine::commitCursorPosition()
{
    // This function makes sure that after a undo the
    // cursor also moves to the position it was just
    // before the undone operation. Therefore, when the
    // cursor is moved, we always need to modify the most
    // previous commit in the undo history.
    Patch *lastPatch = versions[redoPointer]->getPatch();
    lastPatch->switchCurrentSection(currentSectionIndex());
    if (!lastPatch->currentSection()->isEmpty())
        lastPatch->currentSection()->setCursor(currentSection()->cursorPosition());
}
void PatchEditEngine::commitFolding()
{
    // Copy the current folding state fron the current patch to the
    // last committed patch, in order to have undo bring back the
    // same folding state we had just before the undone operation.
    Patch *lastPatch = versions[redoPointer]->getPatch();
    for (int i=0; i<numSections(); i++) {
        PatchSection *lastSection = lastPatch->section(i);
        PatchSection *thisSection = section(i);
        for (unsigned j=0; j<lastSection->numCircuits(); j++) {
            Circuit *lastCircuit = lastSection->circuit(j);
            Circuit *thisCircuit = thisSection->circuit(j);
            lastCircuit->setFold(thisCircuit->isFolded());
        }
    }
}
bool PatchEditEngine::isPatchingFrom(const CursorPosition &pos) const
{
    return isPatching() && patchingStartSection == currentSectionIndex() && patchingStartPosition == pos;
}
void PatchEditEngine::startPatching()
{
    patching = true;
    patchingStartSection = currentSectionIndex();
    patchingStartPosition = currentSection()->cursorPosition();
}
void PatchEditEngine::stopPatching()
{
    patching = false;
}
PatchSection *PatchEditEngine::getPatchingStartSection()
{
    return section(patchingStartSection);
}
