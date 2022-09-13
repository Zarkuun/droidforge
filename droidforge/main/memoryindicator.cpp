#include "memoryindicator.h"
#include "colorscheme.h"
#include "droidfirmware.h"
#include "updatehub.h"
#include "globals.h"

#define MI_WIDTH 200

MemoryIndicator::MemoryIndicator(PatchEditEngine *patch, QWidget *parent)
    : QWidget{parent}
    , PatchView(patch)
    , memoryNeeded(0)
    , memoryAvailable(1)
{
    setMinimumWidth(MI_WIDTH);
    setMaximumWidth(MI_WIDTH);

    connect(the_hub, &UpdateHub::patchModified, this, &MemoryIndicator::updateStatus);
}
void MemoryIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), COLOR(COLOR_STATUSBAR_BACKGROUND));

    float used = float(memoryNeeded) / float(memoryAvailable);
    unsigned perc = 100 * used;
    QRectF barRect = rect().adjusted(2, 1, -2, -1);

    if (memoryNeeded <= memoryAvailable) {
        painter.fillRect(barRect, COLOR(MI_COLOR_BAR));
        QRectF barRectUsed(barRect.left(), barRect.top(), qMin(1.0, used) * barRect.width(), barRect.height());
        painter.fillRect(barRectUsed, COLOR(MI_COLOR_BAR_USED));
    }
    else {
        painter.fillRect(barRect, COLOR(MI_COLOR_BAR_FULL));
    }
    painter.setPen(COLOR(MI_COLOR_BAR_BORDER));
    painter.drawRect(barRect);


    QRectF textRect = rect();
    QString text = tr("%1%").arg(perc);
    painter.setPen(COLOR(MI_COLOR_TEXT));
    painter.drawText(textRect, text, Qt::AlignVCenter | Qt::AlignCenter);
}
void MemoryIndicator::updateStatus()
{
    memoryAvailable = the_firmware->availableMemory();
    memoryNeeded = patch->memoryFootprint();
    QString tooltip = tr("Your circuits and controllers need %1 bytes of memory.").arg(memoryNeeded);
    if (memoryNeeded <= memoryAvailable) {
        unsigned perc = memoryNeeded * 100 / memoryAvailable;
        tooltip += " " + tr("That is %1% of the avaiable memory. You have %2 bytes left.").arg(perc).arg(memoryAvailable - memoryNeeded);
    }
    else {
        tooltip += " " + tr("That is %1 bytes more than there is available! "
                            "Try to remove some circuits.").arg(memoryNeeded - memoryAvailable);
    }
    setToolTip(tooltip);
    update();
}
