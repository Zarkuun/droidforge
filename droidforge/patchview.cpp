#include "patchview.h"
#include "mainwindow.h"
#include "tuning.h"
#include "patch.h"
#include "patchsectionview.h"

#include <QGraphicsItem>
#include <QResizeEvent>

PatchView::PatchView()
    : QTabWidget()
    , currentPatchSectionView(0)
    , patch(0)
    , patchPropertiesDialog{}
    , circuitChooseDialog{}
    , jackChooseDialog{}
{
    grabKeyboard();
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

    while (this->tabBar()->count()) {
        removeTab(0);
    }

    currentPatchSectionView = 0;

    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSection *section = patch->section(i);
        PatchSectionView *psv = new PatchSectionView(section);
        QString title = section->title;
        if (title.isEmpty())
            title = "Circuits";
        addTab(psv, title);
        if (currentPatchSectionView == 0)
            currentPatchSectionView = psv;
    }

    if (patchPropertiesDialog)
        delete patchPropertiesDialog;
    patchPropertiesDialog = new PatchPropertiesDialog(patch, this);
}


bool PatchView::handleKeyPress(int key)
{
    return currentPatchSectionView->handleKeyPress(key);
}


void PatchView::nextSection()
{
    this->setCurrentIndex((currentIndex() + 1) % count());
    currentPatchSectionView = (PatchSectionView *)currentWidget();
}


void PatchView::previousSection()
{
    this->setCurrentIndex((currentIndex() - 1 + count()) % count());
    currentPatchSectionView = (PatchSectionView *)currentWidget();
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
            currentPatchSectionView->addNewCircuit(name, circuitChooseDialog->getJackSelection());
    }
    grabKeyboard();
}

void PatchView::addJack()
{
    QString circuit = currentPatchSectionView->currentCircuitName();
    QStringList usedJacks = currentPatchSectionView->usedJacks();

    releaseKeyboard();
    QString name = JackChooseDialog::chooseJack(circuit, usedJacks);
    if (!name.isEmpty())
        currentPatchSectionView->addNewJack(name);
    grabKeyboard();
}


void PatchView::editValue()
{
    releaseKeyboard();
    currentPatchSectionView->editValue(patch);
    grabKeyboard();
}

void PatchView::editCircuitComment()
{
    releaseKeyboard();
    currentPatchSectionView->editCircuitComment();
    grabKeyboard();
}
