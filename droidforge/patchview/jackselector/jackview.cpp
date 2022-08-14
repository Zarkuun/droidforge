#include "jackview.h"
#include "colorscheme.h"
#include "globals.h"
#include "tuning.h"
#include "droidfirmware.h"
#include "iconbase.h"

#include <QPainter>

JackView::JackView(QString circuit, QString jack, const QStringList *usedJacks, jacktype_t onlyType, bool isInput)
    : jack(jack)
    , isInput(isInput)
    , isSelected(false)
{
    bool allowedByOnlyType = true;
    if (onlyType == JACKTYPE_INPUT && !isInput)
        allowedByOnlyType = false;
    else if (onlyType == JACKTYPE_OUTPUT && isInput)
        allowedByOnlyType = false;

    symbol = the_firmware->jackTypeSymbol(circuit, jack, isInput);
    arraySize = the_firmware->jackArraySize(circuit, jack, isInput);
    shout << "jack is " << jack << "size" << arraySize;
    if (isArray()) {
        active = false;
        for (qsizetype i=0; i<arraySize; i++) {
            QString name = jack + QString::number(i+1);
            activeSubjacks[i] = allowedByOnlyType && !usedJacks->contains(name);
            if (activeSubjacks[i])
                active = true;
        }
    }
    else
        active = allowedByOnlyType && !usedJacks->contains(jack);
}

QRectF JackView::boundingRect() const
{
    unsigned height;
    unsigned width =  JSEL_JACK_WIDTH;
    if (arraySize) {
        height = (1 + ((arraySize + 3) / 4)) * JSEL_JACK_HEIGHT;
    }
    else
        height = JSEL_JACK_HEIGHT;
    QRectF r(0, 0, width, height);
    return r;
}
bool JackView::isActive(int subJack) const
{
    if (subJack >= 0 && isArray())
        return activeSubjacks[subJack];
    else
        return active;
}
void JackView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    // QFontMetrics fm(painter.font());
    // tw = fm.width(sx);

    QRectF rect = boundingRect();
    QRectF symbolRect( rect.left(), rect.top(), JSEL_SYMBOL_WIDTH, JSEL_JACK_HEIGHT);
    QRect textRect(JSEL_JACK_HORIZONTAL_PADDING + JSEL_SYMBOL_WIDTH,
                   JSEL_JACK_VERTICAL_PADDING,
                   JSEL_JACK_WIDTH - JSEL_SYMBOL_WIDTH - JSEL_JACK_HORIZONTAL_PADDING,
                   JSEL_JACK_HEIGHT - 2 * JSEL_JACK_VERTICAL_PADDING);

    painter->setPen(COLOR(JSEL_COLOR_LINE));
    painter->fillRect(rect, COLOR(JSEL_COLOR_JACK_BACKGROUND));
    painter->drawRect(rect);
    painter->fillRect(symbolRect, COLOR(isInput ? JSEL_COLOR_SYMBOL_INPUT : JSEL_COLOR_SYMBOL_OUTPUT));
    painter->drawRect(symbolRect);
    painter->drawImage(symbolRect, IconBase::jackTypeSymbol(symbol));
    QColor activeColor = isInput ? COLOR(CIRV_COLOR_INPUT_JACK) : COLOR(CIRV_COLOR_OUTPUT_JACK);
    painter->setPen(active ? activeColor : COLOR(JSEL_COLOR_JACK_INACTIVE));
    painter->drawText(textRect, jack);

    if (arraySize) {
        for (int i=0; i<(int)arraySize; i++) {
            QRectF r((i%4) * JSEL_JACK_WIDTH /4, (1 + i/4) * JSEL_JACK_HEIGHT, JSEL_JACK_WIDTH/4, JSEL_JACK_HEIGHT);
            QString n = QString::number(i+1);
            painter->setPen(COLOR(JSEL_COLOR_LINE));
            painter->drawRect(r);
            painter->setPen(isActive(i) ? activeColor : COLOR(JSEL_COLOR_JACK_INACTIVE));
            painter->drawText(r, n, Qt::AlignCenter | Qt::AlignVCenter);
        }
    }

    if (isSelected) {
        painter->setPen(isActive(subjack) ? COLOR(COLOR_CURSOR_NORMAL) : COLOR(JSEL_COLOR_CURSOR_INACTIVE));
        if (isArray()) {
            QRectF r((subjack%4) * JSEL_JACK_WIDTH /4, (1 + subjack/4) * JSEL_JACK_HEIGHT, JSEL_JACK_WIDTH/4, JSEL_JACK_HEIGHT);
            painter->drawRect(r);
        }
        else
            painter->drawRect(rect);
    }
}
void JackView::select(int s)
{
    subjack = s;
    isSelected = true;
    jackLine->select(true);
    update();
}
void JackView::deselect()
{
    isSelected = false;
    jackLine->select(false);
    update();
}
