#ifndef DRAGREGISTERINDICATOR_H
#define DRAGREGISTERINDICATOR_H

#include "animatedindicator.h"

class DragRegisterIndicator : public AnimatedIndicator
{
    QPointF endPos;
    bool hits;
    bool suitable;

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    void setEnd(QPointF pos, bool hits, bool s);
};

#endif // DRAGREGISTERINDICATOR_H
