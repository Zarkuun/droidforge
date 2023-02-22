#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "framecursor.h"
#include "iconmarker.h"
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

class MainWindow;
class PatchView;
class PatchOperator;

class PatchSectionView : public QGraphicsView, PatchView
{
    Q_OBJECT

    MainWindow *mainWindow;
    int zoomLevel;
    float zoomFactor;
    bool needScrollbarAdaption;
    bool dragging;

    QList<CircuitView *>circuitViews;
    FrameCursor *frameCursor;

public:
    PatchSectionView(MainWindow *mainWindow, PatchEditEngine *initialPatch);
    ~PatchSectionView();
    bool handleKeyPress(QKeyEvent *event);
    void addNewCircuit(QString name, jackselection_t jackSelection);
    QString currentCircuitName() const;
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
    void clearSettings();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    PatchOperator *theOperator();
    bool handleKeyPress(int key, int modifiers);
    void updateCableIndicator();
    CursorPosition *cursorAtMousePosition(QPoint pos) const;
    void setMouseSelection(const CursorPosition &to);
    void instantCopyFrom(const CursorPosition &from);
    void instantCableFrom(const CursorPosition &to);
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
    void moveCursorTab(int whence);
    void moveCursorHome();
    void moveCursorEnd();
    void editCircuit(int key);
    void editJack(int key);
    void editValueByMouse(CursorPosition &pos);
    void editAtom(int key);
    JackAssignment *buildJackAssignment(const QString &jackName);
    void mousePress(QPoint pos, int button, bool doubleClock);
    void createProblemMarkers();
    void createInfoMarkers();
    void createFoldMarkers();
    void createLEDMismatchMarkers();
    void placeMarker(const CursorPosition &pos, icon_marker_t type, const QString &toolTip = "");

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
    void clickOnIconMarker(const IconMarker *info);
    void editJackCommentAt(const CursorPosition &pos);
    void enableDisableObjects(bool enable);
    void handleRightMousePress(const CursorPosition *curPos);
    void newCircuitAt(int position);
    int getInsertPosition(int ypos);

public slots:
    void keyCaptured(QKeyEvent *);
    void modifyPatch();
    void changeSelection();
    void switchSection();
    void moveCursor();
    void changePatching();

    void editValueByShortcut();
    void newCircuitAtCursor();
    void addJack();
    void zoomReset();
    void zoomIn();
    void zoomOut();
    void foldUnfold();
    void foldUnfoldAll();
    void cut();
    void copy();
    void paste();
    void pasteSmart();
    void addMissingJacks();
    void removeUndefinedJacks();
    void sortJacks();
    void selectAll();
    void disableObjects();
    void enableObjects();
    void startPatching();
    void finishPatching();
    void abortPatching();
    void followCable();
    void followRegister();
    void editJackComment();
    void editLabel();

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
