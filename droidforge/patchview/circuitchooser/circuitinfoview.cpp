#include "circuitinfoview.h"
#include "colorscheme.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QPainter>


CircuitInfoView::CircuitInfoView(QString circuit, QString description, unsigned *width)
    : circuit(circuit)
    , description(description)
    , selected(false)
    , circuitViewWidth(width)
{
}


QRectF CircuitInfoView::boundingRect() const
{
    return QRectF(0, 0, *circuitViewWidth, CICH_CIRCUIT_HEIGHT);
}
void CircuitInfoView::select(bool sel)
{
    selected = sel;
    update();
}
void CircuitInfoView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    unsigned text_x = CICH_ICON_WIDTH + 2*CICH_PADDING + STANDARD_SPACING;
    painter->fillRect(boundingRect(), CICH_CIRCUIT_BACKGROUND_COLOR);

    // Circuit name
    painter->setPen(CICH_COLOR_TITLE);
    painter->drawText(QRect(text_x, CICH_PADDING,
                      *circuitViewWidth - text_x, CICH_TITLE_HEIGHT),
                      Qt::AlignTop,
                      circuit.toUpper() + " - " +
                      the_firmware->circuitTitle(circuit));

    // Description
    QRectF textRect(text_x, CICH_PADDING + CICH_TITLE_HEIGHT,
                    *circuitViewWidth - text_x - CICH_PADDING,
                    CICH_CIRCUIT_HEIGHT - CICH_TITLE_HEIGHT - 2 * CICH_PADDING - 2); // Hack
    // TODO: This -2 is a hack in order to avoid some pixels of a potential third line
    // to be visible. We rather need to check if the text would get a third line and
    // remove that line. And indicate the missing text with "..." or something.
    painter->setPen(CICH_COLOR_DESCRIPTION);
    painter->drawText(textRect,
                      Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
                      the_firmware->circuitDescription(circuit));

    // Icon
    QImage image(QString(CIRCUIT_ICON_PATH +  circuit + CIRCUIT_ICON_SUFFIX));
    painter->drawImage(QRect(CICH_PADDING, CICH_PADDING,
                              CICH_ICON_WIDTH, CICH_ICON_WIDTH), image);

    // Cursor
    if (selected)
    {
        painter->setPen(COLOR(COLOR_CURSOR_NORMAL));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}
