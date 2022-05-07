#include "jackview.h"
#include "tuning.h"

#include <QPainter>

JackView::JackView(QString jack, bool isInput)
    : jack(jack)
    , isInput(isInput)
    , isSelected(false)
{

}

QRectF JackView::boundingRect() const
{
    unsigned height = JSEL_JACK_HEIGHT;
    unsigned width =  JSEL_JACK_WIDTH;
    return QRectF(0, 0, width, height);
}


void JackView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // QFontMetrics fm(painter.font());
    // tw = fm.width(sx);

    QRectF rect = boundingRect();
    painter->setPen(COLOR_LINE);
    painter->fillRect(rect, JSEL_COLOR_JACK_BACKGROUND);
    painter->drawRect(rect);
    painter->setPen(isInput ? COLOR_JACK_INPUT : COLOR_JACK_OUTPUT);
    painter->drawText(
                QRect(JSEL_JACK_HORIZONTAL_PADDING,
                      JSEL_JACK_VERTICAL_PADDING,
                      JSEL_JACK_WIDTH,
                      JSEL_JACK_HEIGHT - 2 * JSEL_JACK_VERTICAL_PADDING),
                      jack);

    if (isSelected) {
        painter->setPen(COLOR_FRAME_CURSOR);
        painter->drawRect(rect);
    }
}


void JackView::select()
{
    isSelected = true;
    update();
}


void JackView::deselect()
{
    isSelected = false;
    update();
}
