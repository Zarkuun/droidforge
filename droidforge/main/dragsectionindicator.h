#ifndef DRAGSECTIONINDICATOR_H
#define DRAGSECTIONINDICATOR_H

#include "animatedindicator.h"

class DragSectionIndicator : public AnimatedIndicator
{
    float insertPosition;
    QRectF sectionRect;
    bool hits;

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    void setSectionRect(QRectF rect);
    void setInsertPos(float yPos, bool h);

private:
    QRectF paintRect() const;
};

#endif // DRAGSECTIONINDICATOR_H
