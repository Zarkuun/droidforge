#include "dragcontrollerindicator.h"
#include "tuning.h"
#include "colorscheme.h"

#include <QPainter>

QRectF DragControllerIndicator::boundingRect() const
{
    QRectF r = paintRect();
    if (insertPosition < r.left())
        r.setLeft(insertPosition - RACV_CONTROLLER_DRAG_WIDTH);
    else if (insertPosition > r.right())
        r.setRight(insertPosition + RACV_CONTROLLER_DRAG_WIDTH);
    return r;
}

void DragControllerIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF pr = paintRect();

    QPen pen;
    pen.setColor(hits
                ? COLOR(RACV_COLOR_DRAGARROW)
                : COLOR(RACV_COLOR_DRAGARROW_UNLOCKED));
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setWidth(RACV_CONTROLLER_DRAG_WIDTH);
    painter->setPen(pen);
    painter->drawRect(pr);

    if (!hits) {
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
    }

    painter->drawLine(insertPosition, pr.top(), insertPosition, pr.bottom());

    if (insertPosition >= pr.left() && insertPosition <= pr.right())
        return;

    float y = (pr.top() + pr.bottom()) / 2;
    float x;
    float phi;
    if (insertPosition < pr.left()) {
        x = pr.left();
        phi = 90;
    }
    else {
        x = pr.right();
        phi = 270;
    }

    painter->drawLine(x, y, insertPosition, y);
    painter->translate(insertPosition, y);
    painter->rotate(phi);
    paintArrowHead(painter, RACV_ARROW_HEAD_SIZE);
}

void DragControllerIndicator::setControllerRect(QRectF rect)
{
    controllerRect = rect;
}

void DragControllerIndicator::setInsertPos(float xPos, bool h)
{
    insertPosition = xPos;
    hits = h;
}

QRectF DragControllerIndicator::paintRect() const
{
    return controllerRect.adjusted(
        -RACV_CONTROLLER_DRAG_WIDTH,
        -RACV_CONTROLLER_DRAG_WIDTH,
        RACV_CONTROLLER_DRAG_WIDTH,
        RACV_CONTROLLER_DRAG_WIDTH);
}
