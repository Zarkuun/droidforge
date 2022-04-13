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
    int currentJack;
    unsigned currentColumn;

public:
    CircuitView(Circuit *circuit);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    unsigned numJackAssignments() { return circuit->numJackAssignments(); };
    void select(unsigned currentJack, unsigned currentColumn);
    void deselect();

private:
    void paintJacks(QPainter *painter, unsigned &line, const QColor &color, unsigned y);
    void paintJack(QPainter *painter, JackAssignment *ja, const QColor textcolor, unsigned y, bool sel);
    void paintOperator(QPainter *painter, unsigned x, unsigned y, QString o, const QColor &color);
};

#endif // CIRCUITVIEW_H
