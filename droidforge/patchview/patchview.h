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
    bool patching;
    unsigned patchingStartSection;
    CursorPosition patchingStartPosition;

public:
    PatchView();
    ~PatchView();
    void setPatch(Patch *patch);
    bool handleKeyPress(QKeyEvent *event);
    const PatchSectionView *currentPatchSectionView() const;
    PatchSectionView *currentPatchSectionView();
    bool clipboardFilled() const;
    bool circuitsInClipboard() const;
    bool circuitsSelected() const;
    int numSections() const;
    void updateRegisterHilites() const;
    void clickOnRegister(AtomRegister ar);
    Patch *integratePatch(Patch *otherpatch);
    Patch *getSelectionAsPatch() const;
    bool interactivelyRemapRegisters(Patch *otherpatch);
    bool isPatching() const { return patching; };
    void abortAllActions();
    // TODO: patching zurücksetzen, wenn sich irgendwas am Patch geändert
    // hat. Am besten per Signal oder so.

public slots:
    void nextSection();
    void previousSection();
    void editProperties();
    void newCircuit();
    void addJack();
    void editValue();
    void startPatching();
    void finishPatching();
    void abortPatching();
    void followInternalCable();
    void editCircuitComment();
    void renameCurrentSection();
    void deleteCurrentSection();
    void moveIntoSection();
    void duplicateSection(int index);
    void mergeSections(int indexa, int indexb);
    void deleteSection(int index);
    void newSectionAfterCurrent();
    void newSectionAt(int index);
    void zoom(int how);
    void cut();
    void copy();
    void paste();
    void pasteSmart();

private slots:
    void renameSection(int index);
    void reorderSections(int fromindex, int toindex);
    void tabContextMenu(int index);

private:
    void copyToClipboard(Clipboard *cb = 0);
    PatchSection *addNewSection(QString name, int index=-1);
    QString sectionName(int index);
};

#endif // PATCHVIEW_H
