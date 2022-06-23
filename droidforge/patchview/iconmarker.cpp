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
    QRectF r(3, 3, size()-6, size()-6);
    painter->drawImage(r, image);
}

QString IconMarker::iconName() const
{
    QString t;
    switch (type) {
    case ICON_MARKER_PROBLEM:     t = "warning"; break;
    case ICON_MARKER_INFO:        t = "info"; break;
    case ICON_MARKER_LEDMISMATCH: t = "%1/ledmismatch"; break;
    case ICON_MARKER_FOLDED:      t = "%1/folded"; break;
    default:
        return "";
    }
    if (!t.contains('%'))
        return t;
    else if (the_colorscheme->isDark())
        return t.arg("dark");
    else
        return t.arg("light");
}

int IconMarker::size() const
{
   if (position.row == ROW_CIRCUIT)
       return CIRV_HEADER_HEIGHT;
   else
       return CIRV_JACK_HEIGHT;
}
