#ifndef PATCHSECTIONMANAGER_H
#define PATCHSECTIONMANAGER_H

#include <QObject>
#include <QGraphicsView>
#include <QList>

#include "framecursor.h"
#include "versionedpatch.h"
#include "patchsectiontitleview.h"

class PatchSectionManager : public QGraphicsView
{
    Q_OBJECT
    VersionedPatch *patch; // borrowed
    QList<PatchSectionTitleView *> titleViews;
    FrameCursor *frameCursor;

public:
    explicit PatchSectionManager(QWidget *parent = nullptr);

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
    void changePatch(VersionedPatch *patch);
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
