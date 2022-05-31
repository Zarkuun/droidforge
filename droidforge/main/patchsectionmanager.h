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

protected:
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void changePatch(VersionedPatch *patch);
    void modifyPatch();
    void switchSection();

    void renameSection();
    void deleteSection();
    void moveIntoSection();
    void duplicateSection();
    void mergeWithLeftSection();
    void mergeWithRightSection();
    void newSectionAfterCurrent();
    // void newSectionAt(int index);

signals:
    void patchModified();
    void sectionSwitched();
};

#endif // PATCHSECTIONMANAGER_H
