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

protected:
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void setNewPatch(VersionedPatch *patch);
    void switchSection(int i);

signals:
    void sectionSwitched(int i);
};

#endif // PATCHSECTIONMANAGER_H
