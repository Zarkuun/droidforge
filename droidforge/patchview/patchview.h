#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"
#include "patchsectionview.h"
#include "circuitchoosedialog.h"
#include "jackchoosedialog.h"
#include "patchpropertiesdialog.h"
#include "clipboard.h"
#include "versionedpatch.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    Q_OBJECT

    VersionedPatch *patch;
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
    void setPatch(VersionedPatch *patch);
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
    void jumpTo(int section, const CursorPosition &curPos);
    // TODO: patching zurücksetzen, wenn sich irgendwas am Patch geändert
    // hat. Am besten per Signal oder so.

private:
    void connectActions();
    void copyToClipboard(Clipboard *cb = 0);
    PatchSection *addNewSection(QString name, int index=-1);
    QString sectionName(int index);
    void patchHasChanged();
    void mergeSections(int indexa, int indexb);
    void zoom(int how);

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
    void renameInternalCable();
    void editCircuitComment();
    void renameSection();
    void deleteSection();
    void moveIntoSection();
    void duplicateSection();
    void mergeWithLeftSection();
    void mergeWithRightSection();
    void newSectionAfterCurrent();
    void newSectionAt(int index);
    void zoomReset();
    void zoomIn();
    void zoomOut();
    void cut();
    void copy();
    void paste();
    void pasteSmart();

private slots:
    void reorderSections(int fromindex, int toindex);
    void tabContextMenu(int index);
    void sectionCursorMoved(const CursorPosition &pos);

signals:
    void cursorMoved(int section, const CursorPosition &pos);
};

#endif // PATCHVIEW_H
