#include "jackcircuitview.h"
#include "tuning.h"

#include <QPainter>

JackCircuitView::JackCircuitView(QString circuit)
    : circuit(circuit)
{
}


QRectF JackCircuitView::boundingRect() const
{
    return QRectF(0, 0, JSEL_CIRCUIT_WIDTH, JSEL_CIRCUIT_HEIGHT);
}


void JackCircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF textrect(0, 0, JSEL_CIRCUIT_WIDTH, JSEL_CIRCUIT_TEXT_HEIGHT);
    painter->fillRect(boundingRect(), JSEL_COLOR_CIRCUIT_BACKGROUND);
    painter->setPen(COLOR_LINE);
    painter->drawRect(boundingRect());
    painter->drawRect(textrect);

    painter->setPen(CICH_COLOR_TITLE);
    painter->drawText(textrect, Qt::AlignCenter | Qt::AlignVCenter, circuit.toUpper());


    QPixmap image(QString(":images/circuits/" +  circuit + ".png"));
    if (image.isNull())
        image = QPixmap(QString(":images/circuits/noicon.png"));

    int x = (JSEL_CIRCUIT_WIDTH - JSEL_CIRCUIT_ICON_WIDTH) / 2;
    int y = (JSEL_CIRCUIT_HEIGHT - JSEL_CIRCUIT_ICON_WIDTH - JSEL_CIRCUIT_TEXT_HEIGHT) / 2 + JSEL_CIRCUIT_TEXT_HEIGHT;
    painter->drawPixmap(
                QRect(x, y,
                      JSEL_CIRCUIT_ICON_WIDTH,
                      JSEL_CIRCUIT_ICON_WIDTH), image);
}
