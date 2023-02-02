#ifndef SMOOTHELLIPSEITEM_H
#define SMOOTHELLIPSEITEM_H

#include <QGraphicsEllipseItem>

class SmoothEllipseItem : public QGraphicsEllipseItem
{
public:
    SmoothEllipseItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

#endif // SMOOTHELLIPSEITEM_H
