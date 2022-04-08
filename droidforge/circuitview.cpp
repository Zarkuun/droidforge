#include "circuitview.h"

#include <QPainter>

#define SIDE_PADDING  10
#define JACK_HEIGHT   16
#define HEADER_HEIGHT 20
#define LINE_WIDTH     1
#define WIDTH        400

CircuitView::CircuitView(Circuit *circuit)
    : circuit(circuit)
{
    qDebug() << "circuit hier: " << QString::number((long)circuit, 16);
}


QRectF CircuitView::boundingRect() const
{
    unsigned num_jacks = circuit->jackAssignments.count();
    unsigned height = HEADER_HEIGHT + JACK_HEIGHT * num_jacks + LINE_WIDTH * (num_jacks + 2);
    return QRectF(0, 0, 400, height);
}


void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QLinearGradient linearGrad(QPointF(100, 100), QPointF(200, 200));
    linearGrad.setColorAt(0, QColor(20, 50, 80));
    linearGrad.setColorAt(0.5, QColor(30, 75, 120));
    linearGrad.setColorAt(1, QColor(40,40,60));

    unsigned x = SIDE_PADDING + LINE_WIDTH;
    unsigned y = LINE_WIDTH;
    painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), linearGrad);
    painter->drawText(QRect(x, y, WIDTH-x, HEADER_HEIGHT), Qt::AlignVCenter, circuit->name.toUpper());
    y += LINE_WIDTH + HEADER_HEIGHT;
    for (qsizetype i=0; i<circuit->jackAssignments.count(); i++) {
        JackAssignment *ja = &circuit->jackAssignments[i];
        painter->drawText(QRect(x, y, WIDTH-x, JACK_HEIGHT), Qt::AlignVCenter, ja->jack);
        painter->drawLine(0, y, WIDTH, y);
        // painter->drawRoundedRect(QRect(x, y, WIDTH-x, JACK_HEIGHT), 5, 5);
        y += LINE_WIDTH + JACK_HEIGHT;
    }
}
