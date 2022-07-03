#include "mousedragger.h"
#include "globals.h"

#include <QGraphicsItem>
#include <QGuiApplication>
#include <QStyleHints>
#include <QTimer>
#include <QApplication>

MouseDragger::MouseDragger(QGraphicsView *gv)
    : graphicsView(gv)
    , leftButtonState(IDLE)
    , hoverItem(0)
    , dragStartItem(0)
{
}
void MouseDragger::mousePress(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        mousePressLeft(event);
    if (event->button() == Qt::RightButton)
        mousePressRight(event);
}
void MouseDragger::mousePressLeft(QMouseEvent *event)
{
    stopHovering();

    if (leftButtonState == IDLE) {
        leftClickPos = event->pos();
        leftButtonState = WAITING_FOR_FIRST_RELEASE;
        QTimer::singleShot(doubleClickTime(), this, &MouseDragger::pressTimeout);
    }
    else if (leftButtonState == WAITING_FOR_DOUBLE_CLICK) {
        leftButtonState = IDLE;
        QGraphicsItem *item = itemAt(leftClickPos);
        if (item)
            emit doubleClickedOnItem(item);
        else
            emit doubleClickedOnBackground();
    }
}
void MouseDragger::mousePressRight(QMouseEvent *event)
{
    auto item = itemAt(event->pos());
    if (item)
        emit menuOpenedOnItem(item);
    else
        emit menuOpenedOnBackground();
}
void MouseDragger::mouseRelease(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        mouseReleaseLeft(event);
    if (event->button() == Qt::RightButton)
        mouseReleaseRight(event);
}
void MouseDragger::mouseReleaseLeft(QMouseEvent *event)
{
    if (leftButtonState == WAITING_FOR_FIRST_RELEASE) {
        leftButtonState = WAITING_FOR_DOUBLE_CLICK;
        QTimer::singleShot(doubleClickTime(), this, &MouseDragger::doubleClickTimedOut);
        return;
    }
    else if (leftButtonState == DRAGGING) {
        stopDragging(event);
    }
    hover(event);
}
void MouseDragger::mouseReleaseRight(QMouseEvent *)
{
}
void MouseDragger::mouseMove(QMouseEvent *event)
{
    int buttons = QApplication::mouseButtons();
    if ((buttons & (Qt::LeftButton | Qt::RightButton)) == 0)
        hover(event);

    switch (leftButtonState) {

    case WAITING_FOR_DOUBLE_CLICK:
        if (distanceFromClick(event->pos()) > doubleClickDistance())
        {
            doLeftClick();
            leftButtonState = IDLE;
        }
        break;

    case WAITING_FOR_FIRST_RELEASE:
        if (distanceFromClick(event->pos()) > dragDistance())
            startDragging();
        break;

    case DRAGGING:
        drag(event);

    case IDLE:
        break;
    }
}
void MouseDragger::cancel()
{
    if (leftButtonState == DRAGGING) {
        leftButtonState = IDLE;
        emit draggingAborted();
    }
    stopHovering();
}
QGraphicsItem *MouseDragger::itemAt(QPoint pos)
{
    int bestPrio = -1;
    QGraphicsItem *bestItem = 0;

    for (auto item: graphicsView->items(pos)) {
        if (item->data(DATA_INDEX_DRAGGER_PRIO).isValid()) {
            int prio = item->data(DATA_INDEX_DRAGGER_PRIO).toInt();
            if (prio > bestPrio) {
                bestPrio = prio;
                bestItem = item;
            }
        }
    }
    return bestItem;
}
void MouseDragger::hover(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (item != hoverItem) {
        if (item)
            startHovering(item);
        else
            stopHovering();
    }
}
void MouseDragger::startHovering(QGraphicsItem *item)
{
    if (hoverItem)
        stopHovering();
    hoverItem = item;
    emit hoveredIn(item);
}
void MouseDragger::stopHovering()
{
    if (hoverItem) {
        emit hoveredOut(hoverItem);
        hoverItem = 0;
    }
}
int MouseDragger::doubleClickTime() const
{
    // TODO: This seems to output a wrong time, at least on Mac.
    // It is always 400.
    // return QGuiApplication::styleHints()->mouseDoubleClickInterval();
    return 200;
}
int MouseDragger::doubleClickDistance() const
{
    return QGuiApplication::styleHints()->mouseDoubleClickDistance();
}
int MouseDragger::dragDistance() const
{
    return QGuiApplication::styleHints()->startDragDistance();
}
int MouseDragger::distanceFromClick(QPoint pos) const
{
    return (leftClickPos - pos).manhattanLength();
}
void MouseDragger::doLeftClick()
{
    QGraphicsItem *item = itemAt(leftClickPos);
    if (item)
        emit clickedOnItem(item);
    else
        emit clickedOnBackground();
}
void MouseDragger::startDragging()
{
    leftButtonState = DRAGGING;
    dragStartItem = itemAt(leftClickPos);
    QPoint pos = graphicsView->mapToScene(leftClickPos).toPoint();
    if (dragStartItem)
        emit itemDragged(dragStartItem, 0, pos);
    else
        emit backgroundDragging(pos, pos);
}
void MouseDragger::drag(QMouseEvent *event)
{
    QPoint pos = graphicsView->mapToScene(event->pos()).toPoint();

    if (!dragStartItem) {
        emit backgroundDragging(graphicsView->mapToScene(leftClickPos).toPoint(), pos);
    }
    else {
        QGraphicsItem *item = itemAt(event->pos());
        if (item && item != dragStartItem)
            emit itemDragged(dragStartItem, item, pos);
        else
            emit itemDragged(dragStartItem, 0, pos);
    }
}
void MouseDragger::stopDragging(QMouseEvent *event)
{
    QPoint pos = graphicsView->mapToScene(event->pos()).toPoint();
    if (!dragStartItem) {
        emit backgroundDraggingStopped(leftClickPos, pos);
    }
    else {
        QGraphicsItem *item = itemAt(event->pos());
        if (item && item != dragStartItem)
            emit itemDraggingStopped(dragStartItem, item, pos);
        else
            emit itemDraggingStopped(dragStartItem, 0, pos);
    }
    leftButtonState = IDLE;
}
void MouseDragger::pressTimeout()
{
    if (leftButtonState == WAITING_FOR_FIRST_RELEASE)
        startDragging();
}
void MouseDragger::doubleClickTimedOut()
{
    if (leftButtonState == WAITING_FOR_DOUBLE_CLICK)
        doLeftClick();
    leftButtonState = IDLE;
}
