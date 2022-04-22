#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    PatchSectionView *currentPatchSectionView;

public:
    PatchView();
    void setPatch(Patch *patch);
    bool handleKeyPress(int key);

public slots:
    void nextSection();
    void previousSection();
};

#endif // PATCHVIEW_H
