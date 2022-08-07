#include "smoothellipseitem.h"

#include <QPainter>

SmoothEllipseItem::SmoothEllipseItem()
{
}

void SmoothEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsEllipseItem::paint(painter, option, widget);
}
