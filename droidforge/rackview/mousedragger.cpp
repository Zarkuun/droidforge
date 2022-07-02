#include "mousedragger.h"
#include "globals.h"

#include <QGraphicsItem>
#include <QGuiApplication>
#include <QStyleHints>
#include <QTimer>

MouseDragger::MouseDragger(QGraphicsView *gv)
    : graphicsView(gv)
    , leftButtonState(IDLE)
    , leftButtonPressed(false)
    , rightButtonPressed(false)
    , hoverItem(0)
{
}
void MouseDragger::mousePress(QMouseEvent *event)
{
    shout << "PRESS" << event;
    if (event->button() == Qt::LeftButton)
        mousePressLeft(event);
    if (event->button() == Qt::RightButton)
        mousePressRight(event);
}
void MouseDragger::mousePressLeft(QMouseEvent *event)
{
    leftButtonPressed = true;
    stopHovering();

    if (leftButtonState == IDLE) {
        leftClickPos = event->pos();
        leftButtonState = WAITING_FOR_FIRST_RELEASE;
    }
    else if (leftButtonState == WAITING_FOR_DOUBLE_CLICK) {
        shout << "DOUBLE!";
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
    rightButtonPressed = true;
    auto item = itemAt(event->pos());
    if (item)
        emit menuOpenedOnItem(item);
    else
        emit menuOpenedOnBackground();
}
void MouseDragger::mouseRelease(QMouseEvent *event)
{
    shout << "RELEASE" << event;
    if (event->button() == Qt::LeftButton)
        mouseReleaseLeft(event);
    if (event->button() == Qt::RightButton)
        mouseReleaseRight(event);
}
void MouseDragger::mouseReleaseLeft(QMouseEvent *event)
{
    leftButtonPressed = false;
    if (leftButtonState == WAITING_FOR_FIRST_RELEASE) {
        leftButtonState = WAITING_FOR_DOUBLE_CLICK;
        shout << "TIMER STARTED.";
        QTimer::singleShot(doubleClickTime(), this, &MouseDragger::doubleClickTimedOut);
        return;
    }
    hover(event);
}
void MouseDragger::mouseReleaseRight(QMouseEvent *)
{
    rightButtonPressed = false;
}
void MouseDragger::mouseMove(QMouseEvent *event)
{
    if (!leftButtonPressed && !rightButtonPressed)
        hover(event);

    if (leftButtonState == WAITING_FOR_DOUBLE_CLICK
        && distanceFromClick(event->pos()) > doubleClickDistance())
    {
        doLeftClick();
        leftButtonState = IDLE;
    }
}
void MouseDragger::cancel()
{
    shout << "CANCEL";
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
    shout << "HOVER" << item;
    emit hoveredIn(item);
}

void MouseDragger::stopHovering()
{
    if (hoverItem) {
        shout << "HOVEREND" << hoverItem;
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

int MouseDragger::distanceFromClick(QPoint pos) const
{
    return (leftClickPos - pos).manhattanLength();
}

void MouseDragger::doLeftClick()
{
    shout << "CLICK";
    QGraphicsItem *item = itemAt(leftClickPos);
    if (item)
        emit clickedOnItem(item);
    else
        emit clickedOnBackground();
}

void MouseDragger::doubleClickTimedOut()
{
    shout << "TIMEOUT";
    if (leftButtonState == WAITING_FOR_DOUBLE_CLICK)
        doLeftClick();
    leftButtonState = IDLE;
}
