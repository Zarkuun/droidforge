#include "mousedragger.h"
#include "globals.h"

#include <QGraphicsItem>

MouseDragger::MouseDragger(QGraphicsView *gv)
    : graphicsView(gv)
{
}
void MouseDragger::mousePress(QMouseEvent *event)
{
    shout << "PRESS" << event;
    if (event->button() == Qt::RightButton) {
        auto item = itemAt(event->pos());
        if (item) {
            shout << "MENU OPEN ON ITEM" << item;
            emit menuOpenedOnItem(item);
        }
        else {
            shout << "MENU OPEN ON BACKGROUND";
            emit menuOpenedOnBackground();
        }
    }
}
void MouseDragger::mouseRelease(QMouseEvent *e)
{
    shout << "RELEASE" << e;
}
void MouseDragger::mouseMove(QMouseEvent *e)
{
    shout << "MOVE" << e;
}
void MouseDragger::cancel()
{
    shout << "CANCEL";
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
