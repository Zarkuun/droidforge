#include "versionedpatch.h"
#include "tuning.h"

#include <QTextStream>
#include <QFile>

VersionedPatch::VersionedPatch(const Patch *patch)
    : redoPointer(-1)
    , versionOnDisk(0)
{
    patch->cloneInto(this);
    commit(); // initial commit, so we always have version 0
}

VersionedPatch::VersionedPatch()
{
    addSection(new PatchSection()); // there always must be one section
}

VersionedPatch::~VersionedPatch()
{
    clearVersions();
}

void VersionedPatch::clearVersions()
{
    for (auto version: versions)
        delete version;
}

void VersionedPatch::startFromScratch()
{
    clear();
    clearVersions();
    redoPointer = -1;
    versionOnDisk = 0;
}

bool VersionedPatch::isModified() const
{
    return versionOnDisk != redoPointer;
}

bool VersionedPatch::saveToFile(QString filePath)
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

void VersionedPatch::commit(QString message)
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

void VersionedPatch::undo()
{
    Q_ASSERT(undoPossible());
    versions[redoPointer--]->getPatch()->cloneInto(this);
}

void VersionedPatch::redo()
{
    Q_ASSERT(redoPossible());
    versions[++redoPointer]->getPatch()->cloneInto(this);
}

bool VersionedPatch::undoPossible() const
{
    return redoPointer > 0;
}

bool VersionedPatch::redoPossible() const
{
    return redoPointer + 1 < versions.size();
}

QString VersionedPatch::nextUndoTitle() const
{
    return versions[redoPointer]->getName();
}

QString VersionedPatch::nextRedoTitle() const
{
    return versions[redoPointer+1]->getName();
}
