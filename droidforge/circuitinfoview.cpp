#include "circuitinfoview.h"
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
                      circuit.toUpper());

    // Description
    painter->setPen(CICH_COLOR_DESCRIPTION);
    painter->drawText(QRect(text_x,
                            CICH_PADDING + CICH_TITLE_HEIGHT,
                            *circuitViewWidth - text_x,
                            CICH_CIRCUIT_HEIGHT - CICH_TITLE_HEIGHT),
                      Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
                      the_firmware->circuitDescription(circuit));

    // Icon
    QPixmap image(QString(":images/circuits/" +  circuit + ".png"));
    if (image.isNull())
        image = QPixmap(QString(":images/circuits/noicon.png"));
    painter->drawPixmap(QRect(CICH_PADDING, CICH_PADDING,
                              CICH_ICON_WIDTH, CICH_ICON_WIDTH), image);

    // Cursor
    if (selected)
    {
        painter->setPen(COLOR_FRAME_CURSOR);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}



