#ifndef JACKCIRCUITVIEW_H
#define JACKCIRCUITVIEW_H

#include <QGraphicsItem>

class JackCircuitView : public QGraphicsItem
{
    QString circuit;

public:
    JackCircuitView(QString circuit);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
};

#endif // JACKCIRCUITVIEW_H
