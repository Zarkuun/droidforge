#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include "circuit.h"

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>

class CircuitView : public QGraphicsItem
{
    Circuit *circuit;

public:
    CircuitView(Circuit *circuit);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
};

#endif // CIRCUITVIEW_H
