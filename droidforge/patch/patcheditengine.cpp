#include "patcheditengine.h"
#include "tuning.h"

#include <QTextStream>
#include <QFile>

PatchEditEngine::PatchEditEngine(const Patch *patch)
    : redoPointer(-1)
    , versionOnDisk(0)
    , patching(false)
{
    patch->cloneInto(this);
    commit(); // initial commit, so we always have version 0
}

PatchEditEngine::PatchEditEngine()
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
}

void PatchEditEngine::startFromScratch()
{
    clear();
    clearVersions();
    redoPointer = -1;
    versionOnDisk = 0;
}

bool PatchEditEngine::isModified() const
{
    return versionOnDisk != redoPointer;
}

bool PatchEditEngine::saveToFile(QString filePath)
{
    QFile file(filePath);
    // TODO: filename
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
          return false;
    QTextStream stream(&file);
    stream << toString();
    stream.flush();
    file.close();
    if (stream.status() == QTextStream::Ok) {
        versionOnDisk = redoPointer;
        setFilePath(filePath);
        return true;
    }
    else
        return false;
}

void PatchEditEngine::commit(QString message)
{
    qDebug() << "COMMIT" << message;
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
    versions[redoPointer--]->getPatch()->cloneInto(this);
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