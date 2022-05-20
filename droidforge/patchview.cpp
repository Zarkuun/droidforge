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

// TODO: Im Undo-State muss man sich auch merken, welche Sektion
// gerade angezeigt wird!

PatchView::PatchView()
    : QTabWidget()
    , patch(0)
    , patchPropertiesDialog{}
    , circuitChooseDialog{}
{
    setMovable(true);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &PatchView::renameSection);
    connect(this, &QTabWidget::tabBarClicked, this, &PatchView::tabContextMenu);
    connect(tabBar(), &QTabBar::tabMoved, this, &PatchView::reorderSections);
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
        PatchSectionView *psv = new PatchSectionView(patch, section);
        QString title = section->getNonemptyTitle();
        addTab(psv, title);
    }

    if (patch->numSections() > 0)
        setCurrentIndex(patch->currentSectionIndex());
}

bool PatchView::handleKeyPress(int key)
{
    bool handled = currentPatchSectionView()->handleKeyPress(key);
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
    PatchSection *section = new PatchSection(newname);
    PatchSectionView *psv = new PatchSectionView(patch, section);
    int i = currentIndex() + 1;
    patch->insertSection(i, section);
    patch->setCurrentSectionIndex(i);
    insertTab(i, psv, newname);
    setCurrentIndex(i);
    the_forge->patchHasChanged();
}

void PatchView::renameSection(int index)
{
    QString oldname =  patch->section(index)->getTitle();
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
