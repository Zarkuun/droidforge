#include "patchview.h"
#include "mainwindow.h"
#include "globals.h"

#include <QMessageBox>

PatchView::PatchView(PatchEditEngine *patch)
    : patch(patch)
{
}

PatchSection *PatchView::section()
{
    return patch->currentSection(); // patch never 0, section never 0
}

const PatchSection *PatchView::section() const
{
    return patch->currentSection(); // patch never 0, section never 0
}
