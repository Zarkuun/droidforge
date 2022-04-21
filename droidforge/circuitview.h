#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include "circuit.h"
#include "cursorposition.h"

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
    unsigned commentHeight() const;
    unsigned contentHeight() const;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    unsigned numJackAssignments() { return circuit->numJackAssignments(); };
    void select(const CursorPosition &);
    void deselect();
    int columnAt(unsigned x);
    int jackAt(unsigned y);

private:
    void paintJacks(QPainter *painter, unsigned &line, unsigned y);
    void paintJack(QPainter *painter, JackAssignment *ja, const QColor color, unsigned y, bool sel);
    void paintOperator(QPainter *painter, unsigned x, unsigned y, QString o, const QColor &color);
};

#endif // CIRCUITVIEW_H
