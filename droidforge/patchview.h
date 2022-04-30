#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"
#include "circuitchoosedialog.h"
#include "patchpropertiesdialog.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    PatchSectionView *currentPatchSectionView;
    Patch *patch;
    PatchPropertiesDialog *patchPropertiesDialog;
    CircuitChooseDialog *circuitChooseDialog;

public:
    PatchView();
    ~PatchView();
    void setPatch(Patch *patch);
    bool handleKeyPress(int key);

public slots:
    void nextSection();
    void previousSection();
    void editProperties();
    void newCircuit();
};

#endif // PATCHVIEW_H
