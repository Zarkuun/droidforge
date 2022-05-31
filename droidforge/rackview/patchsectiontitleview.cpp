#include "patchsectiontitleview.h"
#include "tuning.h"

#include <QPainter>
#include <QCursor>

PatchSectionTitleView::PatchSectionTitleView(const QString title, int width)
    : title(title)
    , width(width)
{
    setCursor(Qt::PointingHandCursor);
}

QRectF PatchSectionTitleView::boundingRect() const
{
    return QRectF(0, 0, width, PSM_SECTION_HEIGHT);
}

void PatchSectionTitleView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->fillRect(boundingRect(), PSM_COLOR_SECTION_BACKGROUND);
    QRectF textRect(PSM_TEXT_PADDING, 0, width - 2 * PSM_TEXT_PADDING, PSM_SECTION_HEIGHT);
    painter->drawText(textRect, Qt::AlignVCenter, title);
}
