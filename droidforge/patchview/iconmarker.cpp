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
    QString t;
    switch (type) {
    case ICON_MARKER_PROBLEM:     t = "problemsmall"; break;
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
