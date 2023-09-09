#include "jackcircuitview.h"
#include "colorscheme.h"
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
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    QRectF textrect(0, 0, JSEL_CIRCUIT_WIDTH, JSEL_CIRCUIT_TEXT_HEIGHT);
    painter->fillRect(boundingRect(), COLOR(JSEL_COLOR_CIRCUIT_BACKGROUND));
    painter->setPen(COLOR(JSEL_COLOR_LINE));
    painter->drawRect(boundingRect());
    painter->drawRect(textrect);

    painter->setPen(COLOR(JSEL_COLOR_CIRCUIT_NAME));
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
