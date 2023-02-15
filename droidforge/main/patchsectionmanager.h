#ifndef PATCHSECTIONMANAGER_H
#define PATCHSECTIONMANAGER_H

#include "framecursor.h"
#include "patcheditengine.h"
#include "patchsectiontitleview.h"
#include "patchview.h"
#include "mousedragger.h"
#include "dragsectionindicator.h"

#include <QObject>
#include <QGraphicsView>
#include <QList>

class MainWindow;
class PatchOperator;

class PatchSectionManager : public QGraphicsView, PatchView
{
    Q_OBJECT
    MainWindow *mainWindow;
    QList<PatchSectionTitleView *> titleViews;
    FrameCursor *frameCursor;
    int lastIndex;
    DragSectionIndicator *dragSectionIndicator;
    MouseDragger dragger;

public:
    explicit PatchSectionManager(MainWindow *mainWindow, PatchEditEngine *patch, QWidget *parent = nullptr);

private:
    PatchOperator *theOperator();
    void rebuildGraphics();
    void updateCursor();
    void switchToSection(int i);
    void switchBackward();
    void switchForward();
    void connectActions();
    void connectDragger();
    void popupSectionMenu(int index=-1);
    void mergeSections(int indexa, int indexb);
    int clickedSectionIndex(QMouseEvent *event);
    int snapSectionInsertPosition(int fromIndex, float y, float *insertSnap) const;
    void newSectionAtIndex(int index);
    void duplicateSection(bool smartly);

protected:
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void modifyPatch();
    void switchSection();

    void renameSection();
    void deleteSection();
    void duplicateSectionDumb();
    void duplicateSectionSmartly();
    void mergeWithPreviousSection();
    void mergeWithNextSection();
    void newSectionAfterCurrent();
    void newSectionAtEnd();
    void saveSectionAsPatch();
    void pasteAsSection();
    void mergeAllSections();
    void moveSectionUp();
    void moveSectionDown();

    // from mouseDragger
    void clickOnItem(QGraphicsItem *item);
    void doubleClickOnItem(QGraphicsItem *item);
    void doubleClickOnBackground();
    void openMenuOnBackground();
    void openMenuOnItem(QGraphicsItem *item);
    void hoverIn(QGraphicsItem *item);
    void hoverOut(QGraphicsItem *item);
    void dragItem(QGraphicsItem *startItem, QGraphicsItem *item, QPoint pos);
    void stopDraggingItem(QGraphicsItem *startItem, QGraphicsItem *item, QPoint pos);
    void abortDragging();

signals:
    void patchModified();
    void sectionSwitched();
};

#endif // PATCHSECTIONMANAGER_H
