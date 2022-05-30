#ifndef PROBLEMMARKER_H
#define PROBLEMMARKER_H

#include <QGraphicsItem>

class ProblemMarker : public QGraphicsItem
{
    unsigned size;

public:
    ProblemMarker(unsigned size, QString toolTip);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
};

#endif // PROBLEMMARKER_H
