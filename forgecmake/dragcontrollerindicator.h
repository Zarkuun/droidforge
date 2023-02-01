#ifndef DRAGCONTROLLERINDICATOR_H
#define DRAGCONTROLLERINDICATOR_H

#include "animatedindicator.h"

class DragControllerIndicator : public AnimatedIndicator
{
    float insertPosition;
    QRectF controllerRect;
    bool hits;

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    void setControllerRect(QRectF rect);
    void setInsertPos(float xPos, bool h);

private:
    QRectF paintRect() const;
};

#endif // DRAGCONTROLLERINDICATOR_H
