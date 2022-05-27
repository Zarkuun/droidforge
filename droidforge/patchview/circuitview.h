#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include "circuit.h"
#include "cursorposition.h"
#include "selection.h"

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsDropShadowEffect>

class CircuitView : public QGraphicsItem
{
    Circuit *circuit;
    unsigned circuitNumber; // within section
    const Selection * const *selection;
    unsigned totalWidth;
    unsigned lineHeight;
    unsigned bottomPadding;
    unsigned jackColumnWidth;
    unsigned atomColumnWidth;
    QGraphicsDropShadowEffect effect;
    bool selected;
    int currentJack;
    unsigned currentColumn;
    QPixmap icon;

public:
    CircuitView(Circuit *circuit, unsigned circuitNumber, const Selection * const *selection, unsigned width, unsigned lineHeight, unsigned bottomPadding);
    QRectF boundingRect() const override;
    static unsigned minimumWidth();
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
    void paintJacks(QPainter *painter);
    void paintJack(QPainter *painter, JackAssignment *ja, const QColor color, unsigned row);
    void paintOperator(QPainter *painter, unsigned x, unsigned y, QString o);
    void paintCursor(QPainter *painter) const;
    QRect headerRect() const;
    QRect commentRect() const;
    QRect jackLineRect(int row) const;
    QRect jackRect(int row) const;
    QRect atomRect(int row, int column) const;
    void paintAtom(QPainter *painter, const QRect &rect, Atom *atom, bool isInput);
    // geometry helpers
    QRectF contentRect() const;
    unsigned contentHeight() const;
    unsigned commentHeight() const;
    unsigned contentWidth() const;
    unsigned columnWidth(int c) const;
    unsigned column123Width() const;
    unsigned columnPosition(int c) const;
    unsigned operatorPosition(int o) const; // o is 0 or 1
};

#endif // CIRCUITVIEW_H
