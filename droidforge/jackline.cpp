#include "jackline.h"
#include "tuning.h"

#include <QPainter>

void JackLine::select(bool s)
{
    isSelected = s;
    update();
}


QRectF JackLine::boundingRect() const
{
    float xa = qMin(start.x(), end.x());
    float xe = qMax(start.x(), end.x());
    float ya = qMin(start.y(), end.y());
    float ye = qMax(start.y(), end.y());
    return QRectF(xa, ya, xe - xa, ye - ya);
}


void JackLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(isSelected ? COLOR_FRAME_CURSOR : JSEL_COLOR_JACK_LINE);
    float np = 1.0 - phase;

    QPoint mid(
         (phase * start.x() + np * end.x()),
         (phase * start.y() + np * end.y()));

    QPainterPath path;
    path.moveTo(start);
    path.quadTo(QPoint(mid.x(), start.y()), mid);
    path.quadTo(QPoint(mid.x(), end.y()), end);
    painter->drawPath(path);
}
