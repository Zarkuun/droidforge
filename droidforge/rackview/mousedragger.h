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
    enum {
        IDLE                        = 0,
        WAITING_FOR_FIRST_RELEASE   = 1,
        DRAGGING                    = 2,
        WAITING_FOR_DOUBLE_CLICK    = 3
    } leftButtonState;
    bool leftButtonPressed;
    bool rightButtonPressed;
    QPoint leftClickPos;
    QGraphicsItem *hoverItem;

public:
    MouseDragger(QGraphicsView *gv);
    void mousePress(QMouseEvent *event);
    void mouseRelease(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
    void cancel();

private:
    void mousePressLeft(QMouseEvent *event);
    void mouseReleaseLeft(QMouseEvent *event);
    void mousePressRight(QMouseEvent *event);
    void mouseReleaseRight(QMouseEvent *event);
    QGraphicsItem *itemAt(QPoint pos);
    void hover(QMouseEvent *event);
    void startHovering(QGraphicsItem *item);
    void stopHovering();
    int doubleClickTime() const;
    int doubleClickDistance() const;
    int distanceFromClick(QPoint pos) const;
    void doLeftClick();

private slots:
    void doubleClickTimedOut();

signals:
    void clickedOnBackground();
    void clickedOnItem(QGraphicsItem *item);
    void doubleClickedOnBackground();
    void doubleClickedOnItem(QGraphicsItem *item);
    void menuOpenedOnBackground();
    void menuOpenedOnItem(QGraphicsItem *item);
    void hoveredIn(QGraphicsItem *item); // always symmetrical ...
    void hoveredOut(QGraphicsItem *item); // ... to hoverout!
    void draggedOnItem(QGraphicsItem *item);
    void draggedFromItem(QGraphicsItem *item);
    void draggedOnBackground(QPoint pos);
    void draggedFinishOnItem(QGraphicsItem *item);
    void draggedFinishOnBackground(QPoint pos);
};

#endif // MOUSEDRAGGER_H
