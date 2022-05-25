#include "patchview.h"
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

Patch *PatchView::integratePatch(Patch *otherpatch)
{
    Patch *newPatch = patch->clone();

    // TODO: It's a hack that we need to get the list of availableRegisters here
    // as an argument. that should be computed by patch, not by rackview.

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

        if (reply == QMessageBox::Cancel) {
            delete newPatch;
            return 0;
        }
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
    qDebug() << "AVAILABLE" << availableRegisters.toString();
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

        if (reply == QMessageBox::Cancel) {
            delete newPatch;
            return 0;
        }
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

                if (reply == QMessageBox::Cancel) {
                    delete newPatch;
                    return 0;
                }
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
    newPatch->integratePatch(otherpatch);
    return newPatch;
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
    the_forge->registerEdit(tr("Move circuits into new section"));
    Clipboard cb;
    copyToClipboard(&cb);
    currentPatchSectionView()->deleteCursorOrSelection();
    addNewSection(newname);
    currentPatchSectionView()->pasteFromClipboard(cb);
    the_forge->patchHasChanged();
}

void PatchView::deleteSection(int index)
{
    QString title = currentPatchSectionView()->getTitle();
    QString actionTitle = tr("deleting patch section '%1'").arg(title);
    the_forge->registerEdit(actionTitle);
    patch->deleteSection(index);
    removeTab(index);
    patch->setCurrentSectionIndex(this->currentIndex());
    the_forge->patchHasChanged();
}

void PatchView::addSection()
{
    QString newname = NameChooseDialog::getName(tr("Add new patch section"), tr("Name:"), SECTION_DEFAULT_NAME);

    if (newname.isEmpty())
        return;

    QString actionTitle = QString("adding new patch section '") + newname + "'";
    the_forge->registerEdit(actionTitle);
    addNewSection(newname);
    the_forge->patchHasChanged();
}

PatchSection *PatchView::addNewSection(QString name)
{
    PatchSection *section = new PatchSection(name);
    PatchSectionView *psv = new PatchSectionView(patch, section, zoomLevel);
    int i = currentIndex() + 1;
    patch->insertSection(i, section);
    patch->setCurrentSectionIndex(i);
    insertTab(i, psv, name);
    setCurrentIndex(i);
    return section;
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
        QString title = patch->section(index)->getNonemptyTitle();

        // Delete
        QAction *actionDelete = new QAction(tr("Delete patch section '%1'").arg(title));
        menu->addAction(actionDelete);
        connect(actionDelete, &QAction::triggered, this, [this,index]() {
            this->deleteSection(index); });

        // TODO:
        // Move right
        // Move left
        // Merge with right
        // Merge with left

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
