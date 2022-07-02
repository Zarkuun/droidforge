#include "mousedragger.h"
#include "globals.h"

#include <QGraphicsItem>

MouseDragger::MouseDragger(QGraphicsView *gv)
    : graphicsView(gv)
    , hoverItem(0)
    , state(IDLE)
{
}
void MouseDragger::mousePress(QMouseEvent *event)
{
    shout << "PRESS" << event;
    stopHovering();
    if (event->button() == Qt::RightButton) {
        auto item = itemAt(event->pos());
        if (item) {
            shout << "MENU OPEN ON ITEM" << item;
            state = MENUOPEN;
            emit menuOpenedOnItem(item);
        }
        else {
            shout << "MENU OPEN ON BACKGROUND";
            state = MENUOPEN;
            emit menuOpenedOnBackground();
        }
    }
}
void MouseDragger::mouseRelease(QMouseEvent *e)
{
    shout << "RELEASE" << e;
    if (state == MENUOPEN)
        state = IDLE;
}
void MouseDragger::mouseMove(QMouseEvent *e)
{
    if (state == IDLE)
        hover(e);
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

void MouseDragger::hover(QMouseEvent *e)
{
    QGraphicsItem *item = itemAt(e->pos());
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
