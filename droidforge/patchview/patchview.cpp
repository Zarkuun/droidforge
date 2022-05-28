#include "patchview.h"
#include "atomcable.h"
#include "cablecolorizer.h"
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
    , patchPropertiesDialog{}
    , circuitChooseDialog{}
    , zoomLevel(0)
    , patching(false)
{
    setMovable(true);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &PatchView::renameSection);
    connect(this, &QTabWidget::tabBarClicked, this, &PatchView::tabContextMenu);
    connect(tabBar(), &QTabBar::tabMoved, this, &PatchView::reorderSections);

    QSettings settings;
    if (settings.contains("patchwindow/zoom"))
        zoomLevel = settings.value("patchwindow/zoom").toInt();
}

PatchView::~PatchView()
{
    if (circuitChooseDialog)
        delete circuitChooseDialog;
    if (patchPropertiesDialog)
        delete patchPropertiesDialog;
}

void PatchView::setPatch(Patch *newPatch)
{
    patch = newPatch;

    while (tabBar()->count())
        removeTab(0);

    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSection *section = patch->section(i);
        PatchSectionView *psv = new PatchSectionView(patch, section, zoomLevel);
        QString title = section->getNonemptyTitle();
        addTab(psv, title);
    }

    if (patch->numSections() > 0)
        setCurrentIndex(patch->currentSectionIndex());

    QStringList allCables = patch->allCables();
    the_cable_colorizer->colorizeAllCables(allCables);
}

bool PatchView::handleKeyPress(QKeyEvent *event)
{
    bool handled = currentPatchSectionView()->handleKeyPress(event);
    return handled;
}

const PatchSectionView *PatchView::currentPatchSectionView() const
{
    return (const PatchSectionView *)currentWidget();
}

PatchSectionView *PatchView::currentPatchSectionView()
{
    return (PatchSectionView *)currentWidget();
}

bool PatchView::clipboardFilled() const
{
    return !clipboard.isEmpty();
}

bool PatchView::circuitsInClipboard() const
{
    return clipboard.numCircuits();
}

bool PatchView::circuitsSelected() const
{
    if (!currentPatchSectionView())
        return false;
    else
        return currentPatchSectionView()->circuitsSelected();
}

int PatchView::numSections() const
{
    if (patch)
        return patch->numSections();
    else
        return 0;
}

void PatchView::updateRegisterHilites() const
{
    if (currentPatchSectionView())
        currentPatchSectionView()->updateRegisterHilites();
}

void PatchView::clickOnRegister(AtomRegister ar)
{
    if (currentPatchSectionView())
        currentPatchSectionView()->clickOnRegister(ar);
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

void PatchView::abortAllActions()
{
    if (patching)
        abortPatching();
    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSectionView *psv = (PatchSectionView *)widget(i);
        psv->clearSelection();
     }
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

Patch *PatchView::getSelectionAsPatch() const
{
    return currentPatchSectionView()->getSelectionAsPatch();
}

void PatchView::nextSection()
{
    int i = (currentIndex() + 1) % count();
    this->setCurrentIndex(i);
    patch->setCurrentSectionIndex(i);
}

void PatchView::previousSection()
{
    int i =(currentIndex() - 1 + count()) % count();
    this->setCurrentIndex(i);
    patch->setCurrentSectionIndex(i);
}

void PatchView::editProperties()
{
    PatchPropertiesDialog::editPatchProperties(patch);
}

void PatchView::newCircuit()
{
    // We reuse the circuit choose dialog because we want it to
    // retain the current selection of cursor, category and stuff.
    if (!circuitChooseDialog)
        circuitChooseDialog = new CircuitChooseDialog(this);

    if (circuitChooseDialog->exec() == QDialog::Accepted) {
        QString name = circuitChooseDialog->getSelectedCircuit();
        if (!name.isEmpty())
            currentPatchSectionView()->addNewCircuit(name, circuitChooseDialog->getJackSelection());
    }
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
    the_forge->updateActions();
    the_forge->cableIndicator()->setPatchingState(true);
}

void PatchView::finishPatching()
{
    Q_ASSERT(patching);
    patching = false;
    the_forge->cableIndicator()->setPatchingState(false);

    Q_ASSERT(patchingStartSection < count());

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
            the_forge->updateActions();
            return;
        }
        cableName = cableName.toUpper();
    }

    the_forge->registerEdit(tr("Creating internal cable '%1'").arg(cableName));
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
    the_forge->updateActions();
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
            qDebug() << "FOUND 1";
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
                qDebug() << "FOUND 2";
                found = true;
                break;
            }
            ++it;
        }
    }

    if (!found)
        return;

    unsigned targetSection = it.sectionIndex();
    setCurrentIndex(targetSection);
    CursorPosition curPos = it.cursorPosition();
    qDebug() << "MOVE TO POS" << curPos.circuitNr << curPos.row << curPos.column;
    currentPatchSectionView()->setCursorPosition(curPos);
}

void PatchView::editCircuitComment()
{
    currentPatchSectionView()->editCircuitComment(0);
}

void PatchView::renameCurrentSection()
{
    renameSection(currentIndex());
}

void PatchView::deleteCurrentSection()
{
    deleteSection(currentIndex());
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

void PatchView::duplicateSection(int index)
{
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
    PatchSectionView *psv = new PatchSectionView(patch, newsection, zoomLevel);
    patch->insertSection(index + 1, newsection);
    patch->setCurrentSectionIndex(index + 1);
    insertTab(index + 1, psv, newname);
    setCurrentIndex(patch->currentSectionIndex());
    delete newpatch;
    the_forge->patchHasChanged();
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

void PatchView::deleteSection(int index)
{
    QString title = currentPatchSectionView()->getTitle();
    QString actionTitle = tr("deleting patch section '%1'").arg(title);
    the_forge->registerEdit(actionTitle);
    patch->deleteSection(index);
    removeTab(index); patch->setCurrentSectionIndex(this->currentIndex());
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
    PatchSectionView *psv = new PatchSectionView(patch, section, zoomLevel);
    patch->insertSection(index, section);
    patch->setCurrentSectionIndex(index);
    insertTab(index, psv, name);
    setCurrentIndex(index);
    return section;
}

QString PatchView::sectionName(int index)
{
    return patch->section(index)->getNonemptyTitle();
}

void PatchView::zoom(int how)
{
    QSettings settings;
    if (how == 0)
        zoomLevel = 0;
    else
        zoomLevel += how; // TODO: Impose some limits
    zoomLevel = qMin(ZOOM_MAX, qMax(ZOOM_MIN, zoomLevel));
    settings.setValue("patchwindow/zoom", zoomLevel);
    if (currentPatchSectionView())
        currentPatchSectionView()->setZoom(zoomLevel);
}

void PatchView::cut()
{
    copyToClipboard();
    currentPatchSectionView()->deleteCursorOrSelection();
}

void PatchView::copy()
{
    copyToClipboard();
    the_forge->updateActions();
}

void PatchView::paste()
{
    currentPatchSectionView()->pasteFromClipboard(clipboard);
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

void PatchView::renameSection(int index)
{
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
        QMenu *menu = new QMenu(this);
        // TODO: Für was war das?
        // QString title = patch->section(index)->getNonemptyTitle();

        // New
        QAction *actionNew = new QAction(tr("New section"));
        menu->addAction(actionNew);
        connect(actionNew, &QAction::triggered, this, [this,index]() {
            this->newSectionAt(index+1); });

        // Duplicate
        QAction *actionDuplicate = new QAction(tr("Duplicate section, adapt registers"));
        menu->addAction(actionDuplicate);
        connect(actionDuplicate, &QAction::triggered, this, [this,index]() {
            this->duplicateSection(index); });

        // Move selection
        menu->addAction(the_forge->action(ACTION_MOVE_INTO_SECTION));

        // Merge with left
        if (index > 0) {
            QAction *actionMergeWithLeft = new QAction(tr("Merge with left section"));
            menu->addAction(actionMergeWithLeft);
            connect(actionMergeWithLeft, &QAction::triggered, this, [this,index]() {
                this->mergeSections(index, index-1); });
        }

        // Merge with right
        if (index < numSections() - 1) {
            QAction *actionMergeWithRight = new QAction(tr("Merge with right section"));
            menu->addAction(actionMergeWithRight);
            connect(actionMergeWithRight, &QAction::triggered, this, [this,index]() {
                this->mergeSections(index, index+1); });
        }

        // Delete
        QAction *actionDelete = new QAction(tr("Delete section"));
        menu->addAction(actionDelete);
        connect(actionDelete, &QAction::triggered, this, [this,index]() {
            this->deleteSection(index); });

        menu->popup(QCursor::pos());
    }
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
