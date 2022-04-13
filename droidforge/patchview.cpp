#include "patchview.h"
#include "tuning.h"
#include "patch.h"
#include "patchsectionview.h"

#include <QGraphicsItem>
#include <QResizeEvent>

PatchView::PatchView(Patch *patch)
    : QTabWidget()
    , currentPatchSectionView(0)
{
    for (qsizetype i=0; i<patch->sections.count(); i++) {
        PatchSection *section = patch->sections[i];
        PatchSectionView *psv = new PatchSectionView(section);
        addTab(psv, section->title);
        if (currentPatchSectionView == 0)
            currentPatchSectionView = psv;
    }
}

bool PatchView::handleKeyPress(int key)
{
    return currentPatchSectionView->handleKeyPress(key);

}
