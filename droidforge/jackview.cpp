#include "jackview.h"
#include "tuning.h"
#include "droidfirmware.h"

#include <QPainter>

JackView::JackView(QString circuit, QString jack, bool isInput)
    : jack(jack)
    , isInput(isInput)
    , isSelected(false)
{
    arraySize = the_firmware->jackArraySize(circuit, jack);
    qDebug() << jack << "von" << circuit << "hat" << arraySize;
}


QRectF JackView::boundingRect() const
{
    unsigned height;
    unsigned width =  JSEL_JACK_WIDTH;
    if (arraySize) {
        qDebug() << "JA!";
        height = (1 + ((arraySize + 3) / 4)) * JSEL_JACK_HEIGHT;
    }
    else
        height = JSEL_JACK_HEIGHT;
    QRectF r(0, 0, width, height);
    if (jack == "value")
        qDebug() << " rect von " << jack << r;
    return r;
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

    if (arraySize) {
        for (unsigned i=0; i<arraySize; i++) {
            QRectF r((i%4) * JSEL_JACK_WIDTH /4, (1 + i/4) * JSEL_JACK_HEIGHT, JSEL_JACK_WIDTH/4, JSEL_JACK_HEIGHT);
            QString n = QString::number(i+1);
            painter->setPen(COLOR_LINE);
            painter->drawRect(r);
            painter->setPen(isInput ? COLOR_JACK_INPUT : COLOR_JACK_OUTPUT);
            painter->drawText(r, n, Qt::AlignCenter | Qt::AlignVCenter);
        }
    }

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
