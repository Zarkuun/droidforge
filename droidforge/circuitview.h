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
    unsigned lineHeight;
    QGraphicsDropShadowEffect effect;
    bool selected;
    int currentJack;
    unsigned currentColumn;

public:
    CircuitView(Circuit *circuit, unsigned lineHeight);
    unsigned commentHeight() const;
    unsigned contentHeight() const;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    unsigned numJackAssignments() { return circuit->numJackAssignments(); };
    QStringList usedJacks() const;
    void select(const CursorPosition &);
    void deselect();
    int columnAt(unsigned x);
    int jackAt(unsigned y);
    QPoint frameCursorPosition() const;

private:
    void paintJacks(QPainter *painter, unsigned &line, unsigned y);
    void paintJack(QPainter *painter, JackAssignment *ja, const QColor color, unsigned y);
    void paintOperator(QPainter *painter, unsigned x, unsigned y, QString o);
    void paintCursor(QPainter *painter) const;
    QRect headerRect() const;
    QRect commentRect() const;
    QRect jackRect(int row) const;
    QRect atomRect(int row, int column) const;
    void paintAtom(QPainter *painter, const QRect &rect, Atom *atom);
};

#endif // CIRCUITVIEW_H
