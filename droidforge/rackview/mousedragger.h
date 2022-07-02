#ifndef MOUSEDRAGGER_H
#define MOUSEDRAGGER_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QObject>

#define DATA_INDEX_DRAGGER_PRIO 10

class MouseDragger : public QObject
{
    Q_OBJECT

    QGraphicsView *graphicsView;

public:
    MouseDragger(QGraphicsView *gv);
    void mousePress(QMouseEvent *e);
    void mouseRelease(QMouseEvent *e);
    void mouseMove(QMouseEvent *e);
    void cancel();

private:
    QGraphicsItem *itemAt(QPoint pos);

signals:
    void clickedOnBackground(QPoint pos);
    void clickedOnItem(QGraphicsItem *item);
    void doubleClickedOnBackground(QPoint pos);
    void doubleClickedOnItem(QGraphicsItem *item);
    void menuOpenedOnBackground();
    void menuOpenedOnItem(QGraphicsItem *item);
    void hoverIn(QGraphicsItem *item);
    void hoverOut(QGraphicsItem *item);
    void draggedOnItem(QGraphicsItem *item);
    void draggedFromItem(QGraphicsItem *item);
    void draggedOnBackground(QPoint pos);
    void draggedFinishOnItem(QGraphicsItem *item);
    void draggedFinishOnBackground(QPoint pos);
};

#endif // MOUSEDRAGGER_H
