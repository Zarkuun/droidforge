#include "circuitinfoview.h"

#include <QPainter>


CircuitInfoView::CircuitInfoView(QString circuit, QString description)
    : circuit(circuit)
    , description(description)
{
}

QRectF CircuitInfoView::boundingRect() const
{
    return QRectF(0, 0, 200, 20);
}

void CircuitInfoView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(0, 0, 20, 30, QColor(255, 140, 120));
    painter->drawText(QRect(0, 0, 200, 20),
                      Qt::AlignVCenter,
                      circuit.toUpper());
}



