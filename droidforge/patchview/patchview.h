#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"
#include "circuitchoosedialog.h"
#include "jackchoosedialog.h"
#include "patchpropertiesdialog.h"
#include "clipboard.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    Patch *patch;
    PatchPropertiesDialog *patchPropertiesDialog;
    CircuitChooseDialog *circuitChooseDialog;
    int zoomLevel;
    Clipboard clipboard;

public:
    PatchView();
    ~PatchView();
    void setPatch(Patch *patch);
    bool handleKeyPress(QKeyEvent *event);
    const PatchSectionView *currentPatchSectionView() const;
    PatchSectionView *currentPatchSectionView();
    bool clipboardFilled() const;
    bool circuitsSelected() const;
    int numSections() const;
    void updateRegisterHilites() const;
    void clickOnRegister(AtomRegister ar);
    Patch *integratePatch(Patch *otherpatch);
    Patch *getSelectionAsPatch() const;

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
    void cut();
    void copy();
    void paste();

private slots:
    void renameSection(int index);
    void reorderSections(int fromindex, int toindex);
    void tabContextMenu(int index);

private:
    void copyToClipboard();
};

#endif // PATCHVIEW_H
