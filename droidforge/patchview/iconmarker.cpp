#include "iconmarker.h"
#include "colorscheme.h"
#include "iconbase.h"
#include "tuning.h"

#include <QPainter>
#include <QCursor>

IconMarker::IconMarker(const CursorPosition &pos, icon_marker_t type, const QString &toolTip)
    : type(type)
    , position(pos)
    , image(IMAGE(iconName()))
{
    setZValue(60);
    setToolTip(toolTip);
    setData(DATA_INDEX_ICON_MARKER, type);
    setCursor(Qt::PointingHandCursor);
}

QRectF IconMarker::boundingRect() const
{
    return QRectF(0, 0, size(), size());
}

void IconMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    QRectF r(4, 4, size()-8, size()-8);
    // painter->setRenderHint(QPainter::Antialiasing);
            //| QPainter::SmoothPixmapTransform);
    painter->drawImage(r, image);
}

QString IconMarker::iconName() const
{
    switch (type) {
    case ICON_MARKER_PROBLEM:     return "problemsmall";
    case ICON_MARKER_INFO:        return "info";
    case ICON_MARKER_LEDMISMATCH: return "ledmismatch";
    case ICON_MARKER_FOLDED:      return "folded";
    default:                      return "";
    }
}

int IconMarker::size() const
{
   if (position.row == ROW_CIRCUIT)
       return CIRV_HEADER_HEIGHT;
   else
       return CIRV_JACK_HEIGHT;
}
