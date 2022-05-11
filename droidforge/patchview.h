#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"
#include "circuitchoosedialog.h"
#include "jackchoosedialog.h"
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
    void addJack();
    void editValue();
    void editCircuitComment();
};

#endif // PATCHVIEW_H
