#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    PatchSectionView *currentPatchSectionView;
    Patch *patch;

public:
    PatchView();
    void setPatch(Patch *patch);
    bool handleKeyPress(int key);

public slots:
    void nextSection();
    void previousSection();
    void editProperties();
};

#endif // PATCHVIEW_H
