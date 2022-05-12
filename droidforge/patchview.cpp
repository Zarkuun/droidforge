#include "patchview.h"
#include "mainwindow.h"
#include "tuning.h"
#include "patch.h"
#include "patchsectionview.h"
#include "renamedialog.h"

#include <QGraphicsItem>
#include <QResizeEvent>

// TODO: Im Undo-State muss man sich auch merken, welche Sektion
// gerade angezeigt wird!

PatchView::PatchView()
    : QTabWidget()
    , patch(0)
    , patchPropertiesDialog{}
    , circuitChooseDialog{}
{
    grabKeyboard();
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &PatchView::renameSection);
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
        PatchSectionView *psv = new PatchSectionView(section);
        QString title = section->getTitle();
        if (title.isEmpty())
            title = "Circuits";
        addTab(psv, title);
    }

    if (patch->numSections() > 0)
        setCurrentIndex(patch->currentSectionIndex());

    if (patchPropertiesDialog)
        delete patchPropertiesDialog;
    patchPropertiesDialog = new PatchPropertiesDialog(patch, this);
}

bool PatchView::handleKeyPress(int key)
{
    return patchSectionView()->handleKeyPress(key);
}

const PatchSectionView *PatchView::patchSectionView() const
{
    return (const PatchSectionView *)currentWidget();
}

PatchSectionView *PatchView::patchSectionView()
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
    patchPropertiesDialog->exec();
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
            patchSectionView()->addNewCircuit(name, circuitChooseDialog->getJackSelection());
    }
    grabKeyboard();
}

void PatchView::addJack()
{
    if (patchSectionView()->isEmpty())
        return;

    QString circuit = patchSectionView()->currentCircuitName();
    QStringList usedJacks = patchSectionView()->usedJacks();

    releaseKeyboard();
    QString name = JackChooseDialog::chooseJack(circuit, "", usedJacks);
    if (!name.isEmpty())
        patchSectionView()->addNewJack(name);
    grabKeyboard();
}

void PatchView::editValue()
{
    releaseKeyboard();
    patchSectionView()->editValue(patch);
    grabKeyboard();
}

void PatchView::editCircuitComment()
{
    releaseKeyboard();
    patchSectionView()->editCircuitComment();
    grabKeyboard();
}

void PatchView::renameCurrentSection()
{
    renameSection(currentIndex());
}

void PatchView::deleteCurrentSection()
{
    QString actionTitle = QString("deleting patch section '") + patchSectionView()->getTitle() + "'";
    the_forge->registerEdit(actionTitle);
    int index = currentIndex();
    patch->deleteSection(index);
    removeTab(index);
    patch->setCurrentSectionIndex(this->currentIndex());
    the_forge->updateActions();
}

void PatchView::addSection()
{
    releaseKeyboard();
    QString newname = RenameDialog::getRenameName(tr("Add new patch section"), tr("Name:"), SECTION_DEFAULT_NAME);
    grabKeyboard();

    if (newname.isEmpty())
        return;

    QString actionTitle = QString("adding new patch section '") + newname + "'";
    the_forge->registerEdit(actionTitle);
    PatchSection *section = new PatchSection(newname);
    PatchSectionView *psv = new PatchSectionView(section);
    int i = currentIndex() + 1;
    patch->insertSection(i, section);
    patch->setCurrentSectionIndex(i);
    insertTab(i, psv, newname);
    setCurrentIndex(i);
    the_forge->updateActions();
}

void PatchView::renameSection(int index)
{
    releaseKeyboard();
    QString oldname =  patch->section(index)->getTitle();
    QString newname = RenameDialog::getRenameName(tr("Rename patch section"), tr("New name:"), oldname);
    if (oldname != newname) {
        QString actionTitle = QString("renaming patch section to '") + newname + "'";
        the_forge->registerEdit(actionTitle);
        patch->section(index)->setTitle(newname);
        this->setTabText(index, newname);
    }
    grabKeyboard();
}
