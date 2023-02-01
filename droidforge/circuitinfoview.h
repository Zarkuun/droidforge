#ifndef CIRCUITINFOVIEW_H
#define CIRCUITINFOVIEW_H

#include <QGraphicsItem>

class CircuitInfoView : public QGraphicsItem
{
    QString circuit;
    bool selected;
    unsigned *circuitViewWidth;

public:
    CircuitInfoView(QString circuit, unsigned *width);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;
    const QString &getCircuit() const { return circuit; };
    void select(bool selected = true);
    void deselect() { select(false); };

private:
    void paintMultilineText(QPainter *painter, unsigned text_x, unsigned text_y, unsigned width, unsigned lines, QString text);
};

#endif // CIRCUITINFOVIEW_H
