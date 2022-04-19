#include "patchview.h"
#include "tuning.h"
#include "patch.h"
#include "patchsectionview.h"

#include <QGraphicsItem>
#include <QResizeEvent>

PatchView::PatchView()
    : QTabWidget()
    , currentPatchSectionView(0)
{
}


void PatchView::setPatch(Patch *patch)
{
    while (this->tabBar()->count()) {
        removeTab(0);
    }

    currentPatchSectionView = 0;

    for (qsizetype i=0; i<patch->sections.count(); i++) {
        PatchSection *section = patch->sections[i];
        PatchSectionView *psv = new PatchSectionView(section);
        QString title = section->title;
        if (title.isEmpty())
            title = "Circuits";
        addTab(psv, title);
        if (currentPatchSectionView == 0)
            currentPatchSectionView = psv;
    }
}


bool PatchView::handleKeyPress(int key)
{
    return currentPatchSectionView->handleKeyPress(key);
}
