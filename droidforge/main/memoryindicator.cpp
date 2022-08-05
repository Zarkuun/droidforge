#include "memoryindicator.h"
#include "colorscheme.h"
#include "updatehub.h"

#define MI_WIDTH 200

MemoryIndicator::MemoryIndicator(PatchEditEngine *patch, QWidget *parent)
    : QWidget{parent}
    , PatchView(patch)
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
    QString text = QString("HALLO");
    painter.drawText(textRect, text, Qt::AlignVCenter | Qt::AlignCenter);
}

void MemoryIndicator::updateStatus()
{
    /* Speicherverbrauch berechnen */
}
