#ifdef NIXDA
#include "patchview.h"
#include "atomcable.h"
#include "mainwindow.h"
#include "tuning.h"
#include "patch.h"
#include "patchsectionview.h"
#include "namechoosedialog.h"

#include <QGraphicsItem>
#include <QResizeEvent>
#include <QTabBar>
#include <QApplication>
#include <QMenu>
#include <QSettings>
#include <QMessageBox>

// TODO: Im Undo-State muss man sich auch merken, welche Sektion
// gerade angezeigt wird!

PatchView::PatchView()
    : QTabWidget()
    , patch(0)
    , patching(false)
{
}

bool PatchView::clipboardFilled() const
{
    return !clipboard.isEmpty();
}

bool PatchView::circuitsInClipboard() const
{
    return clipboard.numCircuits();
}


bool PatchView::interactivelyRemapRegisters(Patch *otherpatch)
{
    // Phase 1: If the other patch defines controllers, we can add these
    // controllers to our patch (and shift all references, of course)
    const QStringList &controllers = otherpatch->allControllers();
    if (!controllers.isEmpty())
    {
        int reply = QMessageBox::question(
                    this,
                    tr("Controllers"),
                    tr("The integrated patch contains controller definitions: %1. "
                       "Do you want to add these definitions to your patch?")
                       .arg(controllers.join(" ").toUpper()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            int numExistingControllers = patch->numControllers();
            for (auto &c: controllers)
                patch->addController(c);
            otherpatch->shiftControllerNumbers(-1, numExistingControllers);
        }
    }

    // Phase 2: Remap non-existing or conflicting registers.
    RegisterList availableRegisters;
    patch->collectAvailableRegisterAtoms(availableRegisters);
    RegisterList occupiedRegisters;
    patch->collectUsedRegisterAtoms(occupiedRegisters);
    RegisterList neededRegisters;
    otherpatch->collectUsedRegisterAtoms(neededRegisters);
    RegisterList atomsToRemap;
    for (auto &reg: neededRegisters) {
        if (occupiedRegisters.contains(reg) || !availableRegisters.contains(reg)) {
            atomsToRemap.append(reg);
        }
        else
            occupiedRegisters.append(reg); // now occupied
    }
    if (atomsToRemap.count()) {
        int reply = QMessageBox::question(
                    this,
                    tr("Register conflicts"),
                    tr("Some of the register references in the integrated patch either do not exist in your "
                       "current rack definition or are already occupied. Shall I try to find useful replacements "
                       "for those?\n\n%1").arg(atomsToRemap.toString()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            RegisterList remapFrom;
            RegisterList remapTo;
            RegisterList remapped;

            for (auto& toRemap: atomsToRemap) {
                for (auto &candidate: availableRegisters) {
                    if (occupiedRegisters.contains(candidate))
                        continue;
                    if (toRemap.getRegisterType() != candidate.getRegisterType())
                        continue;
                    remapFrom.append(toRemap);
                    remapTo.append(candidate);
                    occupiedRegisters.append(candidate);
                    remapped.append(toRemap);
                }
            }

            for (auto& atom: remapped)
                atomsToRemap.removeAll(atom);

            // Apply this remapping
            for (unsigned i=0; i<remapFrom.size(); i++)
                otherpatch->remapRegister(remapFrom[i], remapTo[i]);

            // Phase 2b: Remaining un remapped registers
            if (!atomsToRemap.isEmpty()) {
                int reply = QMessageBox::question(
                            this,
                            tr("Register conflicts"),
                            tr("For some register references I could not find a valid replacement in your patch. "
                               "Shall I remove these references (otherwise I would just leave them as "
                               "they are and you check yourselves later)?\n\n%1").arg(atomsToRemap.toString()),
                            QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes);

                if (reply == QMessageBox::Cancel)
                    return false;

                else if (reply == QMessageBox::Yes) {
                    otherpatch->removeRegisterReferences(
                                atomsToRemap,
                                ControllerRemovalDialog::INPUT_REMOVE,
                                ControllerRemovalDialog::OUTPUT_REMOVE);
                }
            }
        }
    }

    // Phase 3: Cables
    // TODO: Alle Kabel des otherpatch sammeln. KOnflikte finden.
    // Wenn es welche gibt, fragen:
    // - umbenennen
    // - lassen
    // - abbrechen
    return true;
}

void PatchView::jumpTo(int section, const CursorPosition &curPos)
{
    setCurrentIndex(section);
    currentPatchSectionView()->setCursorPosition(curPos);
    patch->switchCurrentSection(section);
}

void PatchView::connectActions()
{
    qDebug() << "ICH CONNECTED";
    qDebug() << "RETURNS:" <<
    connect(the_actions->action(ACTION_CUT), &QAction::triggered, this, &PatchView::cut);
    qDebug() << "ACTION" << the_actions->action(ACTION_CUT);

    CONNECT_ACTION(ACTION_NEW_PATCH_SECTION, &PatchView::newSectionAfterCurrent);
    CONNECT_ACTION(ACTION_DUPLICATE_PATCH_SECTION, &PatchView::duplicateSection);
    CONNECT_ACTION(ACTION_MOVE_INTO_SECTION, &PatchView::moveIntoSection);
    CONNECT_ACTION(ACTION_MERGE_WITH_LEFT_SECTION, &PatchView::mergeWithLeftSection);
    CONNECT_ACTION(ACTION_MERGE_WITH_RIGHT_SECTION, &PatchView::mergeWithRightSection);
    CONNECT_ACTION(ACTION_DELETE_PATCH_SECTION, &PatchView::deleteSection);

    CONNECT_ACTION(ACTION_FOLLOW_INTERNAL_CABLE, &PatchView::followInternalCable);
    CONNECT_ACTION(ACTION_RENAME_INTERNAL_CABLE, &PatchView::renameInternalCable);
    CONNECT_ACTION(ACTION_START_PATCHING, &PatchView::startPatching);
    CONNECT_ACTION(ACTION_FINISH_PATCHING, &PatchView::finishPatching);
    CONNECT_ACTION(ACTION_ABORT_PATCHING, &PatchView::abortPatching);
    CONNECT_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, &PatchView::editCircuitComment);
    CONNECT_ACTION(ACTION_NEW_PATCH_SECTION, &PatchView::newSectionAfterCurrent);
    CONNECT_ACTION(ACTION_DELETE_PATCH_SECTION, &PatchView::deleteSection);
    CONNECT_ACTION(ACTION_RENAME_PATCH_SECTION, &PatchView::renameSection);
    CONNECT_ACTION(ACTION_MOVE_INTO_SECTION, &PatchView::moveIntoSection);

}


Patch *PatchView::integratePatch(Patch *otherpatch)
{
    if (interactivelyRemapRegisters(otherpatch)) {
        Patch *newPatch = patch->clone();
        newPatch->integratePatch(otherpatch);
        return newPatch;
    }
    else
        return 0;
}


void PatchView::addJack()
{
    if (currentPatchSectionView()->isEmpty())
        return;

    QString circuit = currentPatchSectionView()->currentCircuitName();
    QStringList usedJacks = currentPatchSectionView()->usedJacks();

    QString name = JackChooseDialog::chooseJack(circuit, "", usedJacks);
    if (!name.isEmpty())
        currentPatchSectionView()->addNewJack(name);
}

void PatchView::editValue()
{
    currentPatchSectionView()->editValue(0);
}

void PatchView::startPatching()
{
    patching = true;
    patchingStartSection = currentIndex();
    patchingStartPosition = currentPatchSectionView()->getCursorPosition();
    the_forge->cableIndicator()->setPatchingState(true);
    currentPatchSectionView()->updateCursor(); // TODO: This seems to be hack
}

void PatchView::finishPatching()
{
    Q_ASSERT(patching);
    patching = false;
    the_forge->cableIndicator()->setPatchingState(false);

    Q_ASSERT((int)patchingStartSection < count());

    PatchSectionView *startView = (PatchSectionView *)widget(patchingStartSection);
    PatchSection *startSection = patch->section(patchingStartSection);
    CursorPosition startPos = patchingStartPosition;
    const Atom *startAtom = startSection->atomAt(startPos);

    PatchSectionView *endView = currentPatchSectionView();
    PatchSection *endSection = patch->section(currentIndex());
    CursorPosition endPos = currentPatchSectionView()->getCursorPosition();
    const Atom *endAtom = endSection->atomAt(endPos);

    QString cableName;
    if (startAtom && startAtom->isCable())
        cableName = ((AtomCable *)startAtom)->getCable();
    else if (endAtom && endAtom->isCable())
        cableName = ((AtomCable *)endAtom)->getCable();
    else {
        cableName = NameChooseDialog::getName(tr("Create new internal patch cable"), tr("Cable name:"));
        if (cableName == "") {
            return;
        }
        cableName = cableName.toUpper();
    }

    the_forge->registerEdit(tr("creating internal cable '%1'").arg(cableName));
    startSection->setAtomAt(startPos, new AtomCable(cableName));
    endSection->setAtomAt(endPos, new AtomCable(cableName));
    if (startView != endView)
        startView->rebuildPatchSection();
    endView->rebuildPatchSection();
    the_forge->patchHasChanged();
}

void PatchView::abortPatching()
{
    patching = false;
    the_forge->cableIndicator()->setPatchingState(false);
}

void PatchView::followInternalCable()
{
    const Atom *currentAtom = currentPatchSectionView()->currentAtom();
    if (!currentAtom || !currentAtom->isCable())
        return;

    QString name = ((AtomCable *)currentAtom)->getCable();

    bool waitForNext = false;
    bool found = false;

    // First try to find the next atom *after* the current
    Patch::iterator it = patch->begin();
    while (*it) {
        Atom *atom = *it;
        if (atom == currentAtom)
            waitForNext = true;
        else if (waitForNext && atom->isCable() && ((AtomCable *)atom)->getCable() == name) {
            found = true;
            break;
        }
        ++it;
    }

    // Now try from the start of the patch
    if (!found) {
        it = patch->begin();
        while (*it && *it != currentAtom) {
            Atom *atom = *it;
            if (atom->isCable() && ((AtomCable *)atom)->getCable() == name) {
                found = true;
                break;
            }
            ++it;
        }
    }

    if (!found)
        return;

    jumpTo(it.sectionIndex(), it.cursorPosition());
}

void PatchView::renameInternalCable()
{
    currentPatchSectionView()->renameCable();
}

void PatchView::editCircuitComment()
{
    currentPatchSectionView()->editCircuitComment(0);
}

void PatchView::moveIntoSection()
{
    QString newname = NameChooseDialog::getName(tr("Move into new section"), tr("New name:"));
    if (newname.isEmpty())
        return;
    the_forge->registerEdit(tr("moving circuits into new section"));
    Clipboard cb;
    copyToClipboard(&cb);
    currentPatchSectionView()->deleteCursorOrSelection();
    addNewSection(newname, currentIndex() + 1);
    currentPatchSectionView()->pasteFromClipboard(cb);
    the_forge->patchHasChanged();
}

void PatchView::duplicateSection()
{
    int index = currentIndex();
    PatchSection *oldSection = patch->section(index);
    QString newname = NameChooseDialog::getName(
                tr("Duplicate section"),
                tr("New name:"),
                oldSection->getTitle());
    if (newname.isEmpty())
        return;

    Patch *newpatch = new Patch();
    newpatch->addSection(oldSection->clone());
    if (!interactivelyRemapRegisters(newpatch)) {
        delete newpatch;
        return;
    }

    the_forge->registerEdit(tr("duplicating section '%1'").arg(oldSection->getTitle()));
    PatchSection *newsection = newpatch->section(0)->clone();
    PatchSectionView *psv = new PatchSectionView(patch); // TODO patch, newsection, zoomLevel);
    connect(psv, &PatchSectionView::cursorMoved, this, &PatchView::sectionCursorMoved);
    patch->insertSection(index + 1, newsection);
    patch->switchCurrentSection(index + 1);
    insertTab(index + 1, psv, newname);
    setCurrentIndex(patch->currentSectionIndex());
    delete newpatch;
    the_forge->patchHasChanged();
}

void PatchView::mergeWithLeftSection()
{
    mergeSections(currentIndex(), currentIndex() - 1);
}

void PatchView::mergeWithRightSection()
{
    mergeSections(currentIndex(), currentIndex() + 1);
}

void PatchView::mergeSections(int indexa, int indexb)
{
    // Make sure indexa < indexb
    if (indexa > indexb) {
        int x = indexb;
        indexb = indexa;
        indexa = x;
    }
    the_forge->registerEdit(tr("merging sections '%1' and '%2'")
                .arg(sectionName(indexa), sectionName(indexb)));
    patch->mergeSections(indexa, indexb);
    removeTab(indexb);
    setCurrentIndex(patch->currentSectionIndex());
    currentPatchSectionView()->rebuildPatchSection();
    the_forge->patchHasChanged();
}

void PatchView::deleteSection()
{
    int index = currentIndex();
    QString title = currentPatchSectionView()->getTitle();
    QString actionTitle = tr("deleting patch section '%1'").arg(title);
    the_forge->registerEdit(actionTitle);
    patch->deleteSection(index);
    removeTab(index); patch->switchCurrentSection(this->currentIndex());
    the_forge->patchHasChanged();
}

void PatchView::newSectionAfterCurrent()
{
    newSectionAt(currentIndex() + 1);
}


void PatchView::newSectionAt(int index)
{
    QString newname = NameChooseDialog::getName(tr("Add new patch section"), tr("Name:"), SECTION_DEFAULT_NAME);

    if (newname.isEmpty())
        return;

    QString actionTitle = QString("adding new patch section '") + newname + "'";
    the_forge->registerEdit(actionTitle);
    addNewSection(newname, index);
    the_forge->patchHasChanged();
}

PatchSection *PatchView::addNewSection(QString name, int index)
{
    PatchSection *section = new PatchSection(name);
    PatchSectionView *psv = new PatchSectionView(patch); // TODO patch, section, zoomLevel);
    connect(psv, &PatchSectionView::cursorMoved, this, &PatchView::sectionCursorMoved);
    patch->insertSection(index, section);
    patch->switchCurrentSection(index);
    insertTab(index, psv, name);
    setCurrentIndex(index);
    return section;
}

QString PatchView::sectionName(int index)
{
    return patch->section(index)->getNonemptyTitle();
}

void PatchView::patchHasChanged()
{
    the_forge->patchHasChanged();
    currentPatchSectionView()->patchHasChanged();
}

void PatchView::pasteSmart()
{
    Patch *patch = clipboard.getAsPatch();
    if (!interactivelyRemapRegisters(patch)) {
        delete patch;
        return;
    }

    the_forge->registerEdit(tr("Smart pasting %1 circuits").arg(clipboard.getCircuits().count()));
    Clipboard cb(patch->section(0)->circuits);
    currentPatchSectionView()->pasteFromClipboard(cb);
    delete patch;
    the_forge->patchHasChanged();
}

void PatchView::renameSection()
{
    int index = currentIndex();
    QString oldname = patch->section(index)->getTitle();
    QString newname = NameChooseDialog::getName(tr("Rename patch section"), tr("New name:"), oldname);
    if (oldname != newname) {
        QString actionTitle = QString("renaming patch section to '") + newname + "'";
        the_forge->registerEdit(actionTitle);
        patch->section(index)->setTitle(newname);
        this->setTabText(index, newname);
        the_forge->patchHasChanged();
    }
}

void PatchView::reorderSections(int fromindex, int toindex)
{
    the_forge->registerEdit("reordering sections");
    patch->reorderSections(fromindex, toindex);
    the_forge->patchHasChanged();
}

void PatchView::tabContextMenu(int index)
{
    if (QApplication::mouseButtons() == Qt::RightButton) {
        // TODO: Damit sich das Menü auf die richtige Sektion bezieht,
        // muss ich sie selektieren, bevor ich das Menü aufmachen kann!
        // Sonst brauch ich alle Slots doppelt und auch die Aktionen.
        setCurrentIndex(index);

        QMenu *menu = new QMenu(this);
        ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_MOVE_INTO_SECTION, menu);
        ADD_ACTION(ACTION_MERGE_WITH_LEFT_SECTION, menu);
        ADD_ACTION(ACTION_MERGE_WITH_RIGHT_SECTION, menu);
        ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);
        menu->popup(QCursor::pos());
    }
}

void PatchView::sectionCursorMoved(const CursorPosition &pos)
{
    cursor_mode_t mode;

    if (patching)
        mode = CURSOR_PATCHING;
    else if (patch->problemAt(currentIndex(), pos) != "")
        mode = CURSOR_PROBLEM;
    else
        mode = CURSOR_NORMAL;
    currentPatchSectionView()->setCursorMode(mode);
    emit cursorMoved(currentIndex(), pos);
}

void PatchView::copyToClipboard(Clipboard *cb)
{
    currentPatchSectionView()->copyToClipboard(cb ? *cb : clipboard);

    QString info;
    if (clipboard.isEmpty())
        info = "";
    else if (clipboard.numCircuits())
        info = tr("Clipboard: %1 circuits").arg(clipboard.numCircuits());
    else if (clipboard.numJacks())
        info = tr("Clipboard: %1 jack assignments").arg(clipboard.numJacks());
    else if (clipboard.numAtoms())
        info = tr("Clipboard: %1 parameters").arg(clipboard.numAtoms());
    else if (clipboard.isComment())
        info = tr("Clipboard: comment");

    the_forge->updateClipboardInfo(info);
}

#endif
