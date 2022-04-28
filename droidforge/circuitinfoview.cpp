#include "circuitinfoview.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QPainter>


CircuitInfoView::CircuitInfoView(QString circuit, QString description)
    : circuit(circuit)
    , description(description)
{
}


QRectF CircuitInfoView::boundingRect() const
{
    return QRectF(0, 0, CICH_CIRCUIT_WIDTH, CICH_CIRCUIT_HEIGHT);
}


void CircuitInfoView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    unsigned text_x = CICH_ICON_WIDTH + STANDARD_SPACING;
    painter->fillRect(boundingRect(), CICH_CIRCUIT_BACKGROUND_COLOR);

    // Circuit name
    painter->setPen(CICH_COLOR_TITLE);
    painter->drawText(QRect(text_x, 0, CICH_CIRCUIT_WIDTH - text_x, CICH_CIRCUIT_HEIGHT),
                      Qt::AlignTop,
                      circuit.toUpper());

    // Description
    painter->setPen(CICH_COLOR_DESCRIPTION);
    painter->drawText(QRect(text_x,
                            CICH_TITLE_HEIGHT,
                            CICH_CIRCUIT_WIDTH - text_x,
                            CICH_CIRCUIT_HEIGHT - CICH_TITLE_HEIGHT),
                      Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
                      the_firmware->circuitDescription(circuit));

    QPixmap image(QString(":images/circuits/" +  circuit + ".png"));
    if (image.isNull())
        image = QPixmap(QString(":images/circuits/noicon.png"));
    painter->drawPixmap(QRect(0, 0, CICH_ICON_WIDTH, CICH_ICON_WIDTH), image);
}



