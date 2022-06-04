#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "framecursor.h"
#include "iconmarker.h"
#include "infomarker.h"
#include "patchsection.h"
#include "selection.h"
#include "tuning.h"
#include "circuitchoosedialog.h"
#include "atomselectordialog.h"
#include "clipboard.h"
#include "patcheditengine.h"
#include "patchview.h"

#include <QGraphicsView>
#include <QResizeEvent>
#include <QGraphicsRectItem>

class PatchView;

class PatchSectionView : public QGraphicsView, PatchView
{
    Q_OBJECT

    int zoomLevel;
    float zoomFactor;
    bool needScrollbarAdaption;

    QList<CircuitView *>circuitViews;
    AtomSelectorDialog *atomSelectorDialog; // TODO: Jede Section hat nen eigenen Dialog
    FrameCursor frameCursor;

public:
    PatchSectionView(PatchEditEngine *initialPatch);
    ~PatchSectionView();
    bool handleKeyPress(QKeyEvent *event);
    void addNewCircuit(QString name, jackselection_t jackSelection);
    QString currentCircuitName() const;
    QString getTitle() const { return "KEIN TITLE"; }; // ; { return section->getNonemptyTitle(); };
    QStringList usedJacks() const;
    void deleteCursorOrSelection();
    void editValue(int key);
    void editCircuitComment(int key);
    void renameCable();
    bool isEmpty() const;
    void updateCircuits();
    void updateRegisterHilites() const;
    void clickOnRegister(AtomRegister ar);
    void rebuildPatchSection();
    const Atom *currentAtom() const;
    Atom *currentAtom();
    bool atomCellSelected() const;
    // void setCursorPosition(const CursorPosition &pos);
    const CursorPosition &getCursorPosition() const;
    void updateCursor();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private:
    PatchView *patchView();
    void updateCableIndicator();
    CursorPosition *cursorAtMousePosition(QPoint pos) const;
    void setMouseSelection(const CursorPosition &to);
    void updateKeyboardSelection(const CursorPosition &before, const CursorPosition &after);
    void handleLeftMousePress(const CursorPosition &curPos);
    void mouseMoveEvent(QMouseEvent *event);
    void buildPatchSection();
    void deletePatchSection();
    CircuitView *currentCircuitView();
    const CircuitView *currentCircuitView() const;
    Circuit *currentCircuit();
    const Circuit *currentCircuit() const;
    JackAssignment *currentJackAssignment();
    void moveCursorUpDown(int whence);
    void moveCursorLeftRight(int whence);
    void moveCursorPageUpDown(int whence);
    void editCircuit(int key);
    void editJack(int key);
    void editValueByMouse(CursorPosition &pos);
    void editAtom(int key);
    JackAssignment *buildJackAssignment(const QString &jackName);
    QChar keyToChar(int key); // TODO: was soll das hier?
    void mouseClick(QPoint pos, int button, bool doubleClock);
    void updateProblemMarkers();
    void updateInfoMarkers();
    void clearMarkers(int which);
    void placeMarker(const CursorPosition &pos, int which, const QString &toolTip);

    void connectActions();
    void setZoom(int zoom);
    void changeZoom(int how);

    // Delete, Cut, Copy & Paste
    void deleteCurrentRow();
    void deleteCurrentCircuit();
    void deleteMultipleCircuits(int from, int to);
    void deleteCurrentComment();
    void deleteCurrentJack();
    void deleteMultipleJacks(int circuitNr, int from, int to);
    void deleteCurrentAtom();
    void deleteMultipleAtoms(int circuitNr, int row, int from, int to);
    void copyToClipboard();
    void pasteCommentFromClipboard();
    void pasteJacksFromClipboard();
    void pasteAtomsFromClipboard();
    void pasteFromClipboard();
    void pasteCircuitsFromClipboard();
    void clickOnInfoMarker(const InfoMarker *info);
    void editJackCommentAt(const CursorPosition &pos);
    void clearSelection();
    void enableDisableObjects(bool enable);
    void handleRightMousePress(const CursorPosition *curPos);

public slots:
    void modifyPatch();
    void changeSelection();
    void switchSection();
    void moveCursor();
    void changePatching();

    void newCircuit();
    void addJack();
    void zoomReset();
    void zoomIn();
    void zoomOut();
    void cut();
    void copy();
    void paste();
    void pasteSmart();
    void sortJacks();
    void selectAll();
    void disableObjects();
    void enableObjects();
    void createSectionFromSelection();
    void startPatching();
    void finishPatching();
    void abortPatching();
    void followCable();
    void editJackComment();

private slots:
    void clockTick();

signals:
    void patchModified();
    void sectionSwitched();
    void cursorMoved();
    void clipboardChanged();
    void selectionChanged();
    void patchingChanged();
};

#endif // PATCHSECTIONVIEW_H
