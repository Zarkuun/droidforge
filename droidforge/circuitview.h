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
    unsigned currentJack;

public:
    CircuitView(Circuit *circuit);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    unsigned paintJacks(QPainter *painter, unsigned &line, jacktype_t jacktype, const QColor &color, unsigned y);
    unsigned numJackAssignments() { return circuit->numJackAssignments(); };
    void select(unsigned currentJack);
    void deselect();
};

#endif // CIRCUITVIEW_H
