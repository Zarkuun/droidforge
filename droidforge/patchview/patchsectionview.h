#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "patchsection.h"
#include "selection.h"
#include "tuning.h"
#include "circuitchoosedialog.h"
#include "atomselectordialog.h"
#include "clipboard.h"

#include <QGraphicsView>
#include <QResizeEvent>

class PatchSectionView : public QGraphicsView
{
    const Patch *patch;
    PatchSection *section;
    float zoomFactor;

    QList<CircuitView *>circuitViews;
    AtomSelectorDialog *atomSelectorDialog; // TODO: Jede Section hat nen eigenen Dialog
    Selection *selection;

public:
    PatchSectionView(const Patch *patch, PatchSection *section, int zoom);
    ~PatchSectionView();
    bool handleKeyPress(QKeyEvent *event);
    void addNewCircuit(QString name, jackselection_t jackSelection);
    void addNewJack(QString name);
    QString currentCircuitName() const;
    QString getTitle() const { return section->getNonemptyTitle(); };
    QStringList usedJacks() const;
    void deleteCursorOrSelection();
    void pasteFromClipboard(Clipboard &clipboard);
    void pasteCircuitsFromClipboard(const Clipboard &clipboard);
    void editValue(int key);
    void editCircuitComment(int key);
    bool isEmpty() const;
    bool circuitsSelected() const;
    void updateCircuits();
    void updateRegisterHilites() const;
    void clickOnRegister(AtomRegister ar);
    void setZoom(int zoom);
    void copyToClipboard(Clipboard &clipboard);
    Patch *getSelectionAsPatch() const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void updateCursor();
    void setMouseSelection(const CursorPosition &to);
    void updateKeyboardSelection(const CursorPosition &before, const CursorPosition &after);
    void clearSelection();
    bool handleMousePress(const QPointF &pos);
    void buildPatchSection();
    void deletePatchSection();
    void rebuildPatchSection();
    CircuitView *currentCircuitView();
    const CircuitView *currentCircuitView() const;
    Circuit *currentCircuit();
    const Circuit *currentCircuit() const;
    JackAssignment *currentJackAssignment();
    void moveCursorUpDown(int whence);
    void moveCursorLeftRight(int whence);
    void moveCursorPageUpDown(int whence);
    void deleteCurrentRow();
    void deleteCurrentCircuit();
    void deleteMultipleCircuits(int from, int to);
    void deleteCurrentComment();
    void deleteCurrentJack();
    void deleteMultipleJacks(int circuitNr, int from, int to);
    void deleteCurrentAtom();
    void deleteMultipleAtoms(int circuitNr, int row, int from, int to);
    void pasteCommentFromClipboard(const Clipboard &clipboard);
    void pasteJacksFromClipboard(const Clipboard &clipboard);
    void pasteAtomsFromClipboard(const Clipboard &clipboard);
    void editCircuit(int key);
    void editJack(int key);
    void editAtom(int key);
    JackAssignment *buildJackAssignment(const QString &jackName);
    QChar keyToChar(int key);
};

#endif // PATCHSECTIONVIEW_H
