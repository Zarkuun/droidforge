#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include "circuit.h"

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsDropShadowEffect>

class CircuitView : public QGraphicsItem
{
    Circuit *circuit;
    QGraphicsDropShadowEffect effect;
    bool selected;

public:
    CircuitView(Circuit *circuit);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    unsigned paintJacks(QPainter *painter, jacktype_t jacktype, const QColor &color, unsigned y);
    void select();
    void deselect();
};

#endif // CIRCUITVIEW_H
