#include "problemmarker.h"
#include "tuning.h"

#include <QPainter>

ProblemMarker::ProblemMarker(unsigned size, QString toolTip)
    : size(size)
{
    setToolTip(toolTip);
}

QRectF ProblemMarker::boundingRect() const
{
    return QRectF(0, 0, size, size);
}

void ProblemMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    static QImage warningImage(":images/icons/warning.png"); // TODO central storage
    QRectF r(3, 3, size-6, size-6);
    painter->drawImage(r, warningImage);
}
