#include "patcheditengine.h"
#include "tuning.h"

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
    versionOnDisk = -1;
    clear();
    clearVersions();
}

bool PatchEditEngine::isModified() const
{
    return versionOnDisk != redoPointer;
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
    qDebug() << "Situation: redo: " << redoPointer << "versions:" << versions.count() << "title:" << versions.last()->getName();
    qDebug() << "Cursor" << versions[redoPointer]->getPatch()->section(0)->cursorPosition();
    versions[--redoPointer]->getPatch()->cloneInto(this);
}

void PatchEditEngine::redo()
{
    Q_ASSERT(redoPossible());
    versions[++redoPointer]->getPatch()->cloneInto(this);
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
    Patch *lastPatch = versions.last()->getPatch();
    lastPatch->switchCurrentSection(currentSectionIndex());
    lastPatch->currentSection()->setCursor(currentSection()->cursorPosition());
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
