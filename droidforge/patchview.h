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
    Patch *patch;
    PatchPropertiesDialog *patchPropertiesDialog;
    CircuitChooseDialog *circuitChooseDialog;
    int zoomLevel;

public:
    PatchView();
    ~PatchView();
    void setPatch(Patch *patch);
    bool handleKeyPress(int key);
    const PatchSectionView *currentPatchSectionView() const;
    PatchSectionView *currentPatchSectionView();
    int numSections() const;
    void updateRegisterHilites() const;
    void clickOnRegister(AtomRegister ar);
    void integratePatch(const RegisterList &availableRegisters, Patch *otherpatch);

public slots:
    void nextSection();
    void previousSection();
    void editProperties();
    void newCircuit();
    void addJack();
    void editValue();
    void editCircuitComment();
    void renameCurrentSection();
    void deleteCurrentSection();
    void deleteSection(int index);
    void addSection();
    void zoom(int how);

private slots:
    void renameSection(int index);
    void reorderSections(int fromindex, int toindex);
    void tabContextMenu(int index);
};

#endif // PATCHVIEW_H
