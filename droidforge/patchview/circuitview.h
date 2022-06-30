#ifndef CIRCUITVIEW_H
#define CIRCUITVIEW_H

#include "circuit.h"
#include "cursorposition.h"
#include "selection.h"

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsDropShadowEffect>

class CircuitView : public QObject, public QGraphicsItem
{
    Circuit *circuit;
    unsigned circuitNumber; // within section
    const Selection * const *selection;
    float totalWidth;
    float lineHeight;
    float jackColumnWidth;
    float atomColumnWidth;
    QPixmap icon;
    QImage  iconImage;
    int markerOffset;

public:
    CircuitView(Circuit *circuit, unsigned circuitNumber, const Selection * const *selection, float width, unsigned lineHeight);
    QRectF boundingRect() const override;
    static unsigned minimumWidth();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    QRectF cellRect(int row, int column) const;
    unsigned numJackAssignments() { return circuit->numJackAssignments(); };
    QStringList usedJacks() const;
    int columnAt(unsigned x);
    int jackAt(unsigned y);
    int nextHeaderMarkerOffset();

private:
    bool isFolded() const { return circuit->isFolded(); };
    void paintJacks(QPainter *painter);
    void paintJack(QPainter *painter, JackAssignment *ja, unsigned row);
    void paintOperator(QPainter *painter, unsigned x, unsigned y, QString o);
    QRectF headerRect() const;
    QRectF commentRect() const;
    QRectF jackLineRect(int row) const;
    QRectF jackRect(int row) const;
    QRectF atomRect(int row, int column) const;
    void paintAtom(QPainter *painter, const QRectF &rect, QColor textcolor, Atom *atom, bool isInput);
    // geometry helpers
    float totalHeight() const;
    float commentHeight() const;
    float columnWidth(int c) const;
    float column123Width() const;
    float columnPosition(int c) const;
    float operatorPosition(int o) const; // o is 0 or 1
    void paintHeader(QPainter *painter);
    void paintComment(QPainter *painter);
    void paintLines(QPainter *painter);
    void paintSelection(QPainter *painter);
};

#endif // CIRCUITVIEW_H
