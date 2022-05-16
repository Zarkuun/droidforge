#include "patchview.h"
#include "mainwindow.h"
#include "tuning.h"
#include "patch.h"
#include "patchsectionview.h"
#include "namechoosedialog.h"

#include <QGraphicsItem>
#include <QResizeEvent>
#include <QTabBar>

// TODO: Im Undo-State muss man sich auch merken, welche Sektion
// gerade angezeigt wird!

PatchView::PatchView()
    : QTabWidget()
    , patch(0)
    , patchPropertiesDialog{}
    , circuitChooseDialog{}
{
    grabKeyboard();
    setMovable(true);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &PatchView::renameSection);
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

    while (tabBar()->count()) {
        removeTab(0);
    }

    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSection *section = patch->section(i);
        PatchSectionView *psv = new PatchSectionView(patch, section);
        QString title = section->getTitle();
        if (title.isEmpty())
            title = "Circuits";
        addTab(psv, title);
    }

    if (patch->numSections() > 0)
        setCurrentIndex(patch->currentSectionIndex());
}

bool PatchView::handleKeyPress(int key)
{
    releaseKeyboard(); // TODO: How can we get rid of this "grab keyboard" hack?
    bool handled = currentPatchSectionView()->handleKeyPress(key);
    grabKeyboard();
    return handled;
}

const PatchSectionView *PatchView::patchSectionView() const
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
    releaseKeyboard();
    PatchPropertiesDialog::editPatchProperties(patch);
    grabKeyboard();
}

void PatchView::newCircuit()
{
    releaseKeyboard();
    // We reuse the circuit choose dialog because we want it to
    // retain the current selection of cursor, category and stuff.
    if (!circuitChooseDialog)
        circuitChooseDialog = new CircuitChooseDialog(this);

    if (circuitChooseDialog->exec() == QDialog::Accepted) {
        QString name = circuitChooseDialog->getSelectedCircuit();
        if (!name.isEmpty())
            currentPatchSectionView()->addNewCircuit(name, circuitChooseDialog->getJackSelection());
    }
    grabKeyboard();
}

void PatchView::addJack()
{
    if (currentPatchSectionView()->isEmpty())
        return;

    QString circuit = currentPatchSectionView()->currentCircuitName();
    QStringList usedJacks = currentPatchSectionView()->usedJacks();

    releaseKeyboard();
    QString name = JackChooseDialog::chooseJack(circuit, "", usedJacks);
    if (!name.isEmpty())
        currentPatchSectionView()->addNewJack(name);
    grabKeyboard();
}

void PatchView::editValue()
{
    releaseKeyboard();
    currentPatchSectionView()->editValue(0);
    grabKeyboard();
}

void PatchView::editCircuitComment()
{
    releaseKeyboard();
    currentPatchSectionView()->editCircuitComment(0);
    grabKeyboard();
}

void PatchView::renameCurrentSection()
{
    renameSection(currentIndex());
}

void PatchView::deleteCurrentSection()
{
    QString actionTitle = QString("deleting patch section '") + currentPatchSectionView()->getTitle() + "'";
    the_forge->registerEdit(actionTitle);
    int index = currentIndex();
    patch->deleteSection(index);
    removeTab(index);
    patch->setCurrentSectionIndex(this->currentIndex());
    the_forge->patchHasChanged();
}

void PatchView::addSection()
{
    releaseKeyboard();
    QString newname = NameChooseDialog::getName(tr("Add new patch section"), tr("Name:"), SECTION_DEFAULT_NAME);
    grabKeyboard();

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
    releaseKeyboard();
    QString oldname =  patch->section(index)->getTitle();
    QString newname = NameChooseDialog::getName(tr("Rename patch section"), tr("New name:"), oldname);
    if (oldname != newname) {
        QString actionTitle = QString("renaming patch section to '") + newname + "'";
        the_forge->registerEdit(actionTitle);
        patch->section(index)->setTitle(newname);
        this->setTabText(index, newname);
        the_forge->patchHasChanged();
    }
    grabKeyboard();
}

void PatchView::reorderSections(int fromindex, int toindex)
{
    the_forge->registerEdit("reordering sections");
    patch->reorderSections(fromindex, toindex);
    the_forge->patchHasChanged();
}
