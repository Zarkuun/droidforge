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
    setToolTip("");
    QPainter painter(this);
    painter.fillRect(rect(), COLOR(COLOR_STATUSBAR_BACKGROUND));

    QRectF textRect = rect();
    unsigned memoryAvailable = the_firmware->availableMemory();
    QString text = tr("%1 / %2").arg(memoryNeeded).arg(memoryAvailable);
    painter.drawText(textRect, text, Qt::AlignVCenter | Qt::AlignCenter);
}
void MemoryIndicator::updateStatus()
{
    memoryNeeded = patch->memoryFootprint();
    update();
}
