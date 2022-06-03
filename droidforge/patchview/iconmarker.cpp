#include "iconmarker.h"

#include <QPainter>

IconMarker::IconMarker(const QImage &image, unsigned size, QString toolTip)
    : size(size)
    , image(image)
{
    setToolTip(toolTip);
    setZValue(60);
}

QRectF IconMarker::boundingRect() const
{
    return QRectF(0, 0, size, size);
}

void IconMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF r(3, 3, size-6, size-6);
    painter->drawImage(r, image);
}
