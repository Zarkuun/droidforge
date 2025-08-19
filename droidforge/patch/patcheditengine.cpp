#include "patcheditengine.h"
#include "tuning.h"
#include "globals.h"
#include "hintdialog.h"

#include <QTextStream>
#include <QFile>
#include <QTime>
#include <QSettings>


// Does *not* do a commit()!
PatchEditEngine::PatchEditEngine()
    : redoPointer(-1)
    , versionOnDisk(-1)
    , patching(false)
    , problemsDirtySince(1)
{
    addSection(new PatchSection()); // there always must be one section
}

PatchEditEngine::~PatchEditEngine()
{
    clearVersions();
}
void PatchEditEngine::clearVersions()
{
    for (auto &version: versions)
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
    if (saveContentsToFile(filePath, toString())) {
        versionOnDisk = redoPointer;
        setFilePath(filePath);
        return true;
    }
    else
        return false;
}
void PatchEditEngine::commit(QString message)
{
    sectionCopyDetection();

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

    setProblemsDirty();
}
void PatchEditEngine::undo()
{
    Q_ASSERT(undoPossible());
    versions[--redoPointer]->getPatch()->cloneInto(this);
    setProblemsDirty();
}
void PatchEditEngine::redo()
{
    Q_ASSERT(redoPossible());
    versions[++redoPointer]->getPatch()->cloneInto(this);
    setProblemsDirty();
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
void PatchEditEngine::setProblemsDirty()
{
    problemsDirtySince = QDateTime::currentMSecsSinceEpoch();
}
void PatchEditEngine::checkUpdateProblemsNow()
{
    if (problemsDirtySince != 0)
        updateProblems();
}
bool PatchEditEngine::checkUpdateProblems()
{
    if (problemsDirtySince != 0 && QDateTime::currentMSecsSinceEpoch() - problemsDirtySince > PROBLEMS_UPDATE_DELAY) {
        updateProblems();
        problemsDirtySince = 0;
        return true;
    }
    else
        return false;
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
void PatchEditEngine::sectionCopyDetection()
{
    if (redoPointer < 0)
        return; // empty patch

    Patch *lastPatch = versions[redoPointer]->getPatch();
    if (lastPatch->numSections() != numSections())
        return;

    // Find all sections that where copies of the current one *before*
    // the edit.
    int si = currentSectionIndex();
    PatchSection *mySectionBefore = lastPatch->section(si);

    QSettings settings;
    bool syncClonedSections = settings.value("editing/sync_cloned_sections", false).toBool();

    QString syncedHint;

    for (int s=0; s<numSections(); s++) {
        PatchSection *otherSectionBefore = lastPatch->section(s);
        if (s != si) {
            QString myPrefix;
            QString otherPrefix;
            if (mySectionBefore->isCopyOf(otherSectionBefore, myPrefix, otherPrefix)) {
                // So it was a copy (modulo cable name prefixes) of the other section
                // before. Now check it it's still one afterwards.
                PatchSection *mySectionAfter = section(si);
                PatchSection *otherSectionAfter = section(s);
                QString ignoreA, ignoreB;
                if (!mySectionAfter->isCopyOf(otherSectionAfter, ignoreA, ignoreB)) {
                    if (mySectionBefore->getTitle() != mySectionAfter->getTitle() ||
                        otherSectionBefore->getTitle() != otherSectionAfter->getTitle())
                    {
                    }
                    else {
                        if (syncClonedSections) {
                            intelligentSectionSync(mySectionAfter, s, myPrefix, otherPrefix);
                            switchCurrentSection(si); // got changed by insertSection()
                            if (syncedHint != "")
                                syncedHint += ", ";
                            syncedHint += section(s)->getNonemptyTitle();
                        }
                        else {
                            HintDialog::hint("section_sync_disabled",
                               TR("The section %1 looks very similar to the section you have edited.\n"
                                  "By enabling the global option \"Keep cloned sections in sync\"\n"
                                  "You can have me apply your change to that section, as well.\n\n"
                                  "If you want this, undo your change, enable that option and do your\n"
                                  "change, again.\n").arg(section(s)->getNonemptyTitle()));
                        }
                    }
                }
            }
        }
    }

    if (syncedHint != "") {
        HintDialog::hint("cloned_section_synced",
                         TR("Your change has automatically been synced to\n"
                            "the section(s) %1\n").arg(syncedHint));
    }
}
void PatchEditEngine::intelligentSectionSync(const PatchSection *source, int targetIndex, QString fromPrefix, QString toPrefix)
{
    const PatchSection *overwrittenSection = section(targetIndex);
    PatchSection *cloneSection = source->clone();
    cloneSection->setTitle(overwrittenSection->getTitle());
    cloneSection->setComment(overwrittenSection->getComment());
    cloneSection->rewriteCablePrefixes(fromPrefix, toPrefix);
    removeSection(targetIndex);
    insertSection(targetIndex, cloneSection);
}
