#ifndef CIRCUITINFOVIEW_H
#define CIRCUITINFOVIEW_H

#include <QGraphicsItem>

class CircuitInfoView : public QGraphicsItem
{
    QString circuit;
    QString description;

public:
    CircuitInfoView(QString circuit, QString description);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    QRectF boundingRect() const override;
};

#endif // CIRCUITINFOVIEW_H
