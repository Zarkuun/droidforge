#ifndef CIRCUITINFOVIEW_H
#define CIRCUITINFOVIEW_H

#include <QGraphicsItem>

class CircuitInfoView : public QGraphicsItem
{
    QString circuit;
    QString description;
    bool selected;
    unsigned *circuitViewWidth;

public:
    CircuitInfoView(QString circuit, QString description, unsigned *width);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;
    const QString &getCircuit() const { return circuit; };
    void select(bool selected = true);
    void deselect() { select(false); };
};

#endif // CIRCUITINFOVIEW_H