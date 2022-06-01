#ifndef PATCHSECTIONMANAGER_H
#define PATCHSECTIONMANAGER_H

#include <QObject>
#include <QGraphicsView>
#include <QList>

#include "framecursor.h"
#include "patcheditengine.h"
#include "patchsectiontitleview.h"
#include "patchoperator.h"

class PatchSectionManager : public QGraphicsView, PatchOperator
{
    Q_OBJECT
    QList<PatchSectionTitleView *> titleViews;
    FrameCursor *frameCursor;
    int lastIndex;

public:
    explicit PatchSectionManager(PatchEditEngine *patch, QWidget *parent = nullptr);

private:
    void rebuildGraphics();
    void updateCursor();
    void switchToSection(int i);
    void switchBackward();
    void switchForward();
    void connectActions();
    void popupSectionMenu(int index=-1);
    void mergeSections(int indexa, int indexb);
    int clickedSectionIndex(QMouseEvent *event);

protected:
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void modifyPatch();
    void switchSection();

    void renameSection();
    void deleteSection();
    void duplicateSection();
    void mergeWithPreviousSection();
    void mergeWithNextSection();
    void newSectionAfterCurrent();
    void mergeAllSections();

signals:
    void patchModified();
    void sectionSwitched();
};

#endif // PATCHSECTIONMANAGER_H
