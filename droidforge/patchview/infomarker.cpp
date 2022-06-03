#include "infomarker.h"
#include "tuning.h"

#include <QPainter>
#include <QCursor>


QImage infoImage(":images/icons/info.png"); // TODO central storage

InfoMarker::InfoMarker(const CursorPosition &curPos, unsigned size, QString toolTip)
    : IconMarker(infoImage, size, toolTip)
    , curPos(curPos)
{
    setCursor(Qt::PointingHandCursor); // TODO: Warum geht das hier nicht?
}
