#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    PatchSectionView *currentPatchSectionView;

public:
    PatchView(Patch *patch);
    bool handleKeyPress(int key);
};

#endif // PATCHVIEW_H
