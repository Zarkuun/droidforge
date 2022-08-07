#include "patchsectiontitleview.h"
#include "colorscheme.h"
#include "iconbase.h"
#include "tuning.h"
#include "globals.h"

#include <QPainter>
#include <QCursor>

PatchSectionTitleView::PatchSectionTitleView(const QString &title, int width, unsigned problems)
    : title(title)
    , width(width)
    , problems(problems)
{
    setCursor(Qt::PointingHandCursor);
    if (problems)
        setToolTip(TR("There are problems in this section"));
}

QRectF PatchSectionTitleView::boundingRect() const
{
    return QRectF(0, 0, width, PSM_SECTION_HEIGHT);
}

void PatchSectionTitleView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    painter->fillRect(boundingRect(), COLOR(PSM_COLOR_SECTION_BACKGROUND));
    QRectF textRect(PSM_TEXT_PADDING, 0, width - 2 * PSM_TEXT_PADDING, PSM_SECTION_HEIGHT);
    painter->drawText(textRect, Qt::AlignVCenter, title);
    if (problems) {
        float imgHeight = PSM_SECTION_HEIGHT - 2 * PSM_IMAGE_MARGIN;
        QRectF warnRect(width - imgHeight - PSM_IMAGE_MARGIN,
                        PSM_IMAGE_MARGIN,
                        imgHeight,
                        imgHeight);
        painter->drawImage(warnRect, IMAGE("warning"));
    }
}
