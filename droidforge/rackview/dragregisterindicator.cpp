#include "dragregisterindicator.h"
#include "tuning.h"
#include "colorscheme.h"

#include <QPainter>

// TODO: tuning.h
const int DRI_CIRCLE_SIZE = 2.5 * RACV_PIXEL_PER_HP;

QRectF DragRegisterIndicator::boundingRect() const
{
    QPoint safety(DRI_CIRCLE_SIZE, DRI_CIRCLE_SIZE);
    QRectF box = QRectF(QPoint(0, 0), endPos).normalized();
    return QRectF(box.topLeft() - safety, box.bottomRight() + safety);
}
void DragRegisterIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    QPen pen;
    pen.setWidth(4);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::MiterJoin);

    if (!hits || endPos.isNull()) {
        pen.setColor(COLOR(RACV_COLOR_DRAGARROW_UNLOCKED));
        pen.setStyle(Qt::DotLine);
        painter->setBrush(Qt::NoBrush);
    }
    else {
        if (suitable)
            pen.setColor(COLOR(RACV_COLOR_DRAGARROW));
        else
            pen.setColor(COLOR(RACV_COLOR_DRAGARROW_UNSUITABLE));
        pen.setStyle(Qt::SolidLine);
        QColor fill = pen.color();
        fill.setAlpha(fill.alpha()/3);
        painter->setBrush(fill);
    }
    painter->setPen(pen);

    QRectF a(-DRI_CIRCLE_SIZE/2, -DRI_CIRCLE_SIZE/2, DRI_CIRCLE_SIZE, DRI_CIRCLE_SIZE);
    painter->drawEllipse(a);
    if (endPos.isNull())
        return;

    painter->save();
    painter->translate(endPos);
    painter->drawEllipse(a);
    painter->restore();

    painter->drawLine(QPoint(0, 0), endPos);

    float phi;
    if (endPos.x() > 0) {
        phi = atan(endPos.y() / endPos.x()) * 180 / 3.141 + 90;
    }
    else if (endPos.x() < 0) {
        phi = atan(endPos.y() / endPos.x()) * 180 / 3.141 - 90;
    }
    else if (endPos.y() > 0)
        phi = 180;
    else
        phi = 0;

    const int ARROW_HEAD_SIZE = 0.6 * RACV_PIXEL_PER_HP;
    painter->save();
    painter->rotate(phi);
    paintArrowHead(painter, ARROW_HEAD_SIZE);
    painter->restore();
    painter->save();
    painter->translate(endPos);
    painter->rotate(phi + 180);
    paintArrowHead(painter, ARROW_HEAD_SIZE);
    painter->restore();
}
void DragRegisterIndicator::paintArrowHead(QPainter *painter, float size)
{
    QPolygon poly;
    poly << QPoint(0, 0)
         << QPoint(-size, -2 * size)
         << QPoint( size, -2 * size)
         << QPoint(0,0);
    // QColor fill = painter->pen().color();
    // fill.setAlpha(fill.alpha() / 2);
    // painter->setBrush(fill);
    painter->drawPolygon(poly);
}
void DragRegisterIndicator::setEnd(QPointF pos, bool h, bool s)
{
    endPos = pos;
    hits = h;
    suitable = s;
}
