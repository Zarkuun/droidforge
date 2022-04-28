#ifndef CIRCUITINFOVIEW_H
#define CIRCUITINFOVIEW_H

#include <QGraphicsItem>

class CircuitInfoView : public QGraphicsItem
{
    QString circuit;
    QString description;

public:
    CircuitInfoView(QString circuit, QString description);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;
    const QString &getCircuit() const { return circuit; };
};

#endif // CIRCUITINFOVIEW_H
