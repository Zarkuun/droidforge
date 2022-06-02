#ifndef DRAGREGISTERINDICATOR_H
#define DRAGREGISTERINDICATOR_H

#include <QGraphicsItem>

class DragRegisterIndicator : public QGraphicsItem
{
    QPointF endPos;
    bool hits;
    bool suitable;

public:
    DragRegisterIndicator();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    void setEnd(QPointF pos, bool hits, bool s);

private:
    void paintArrowHead(QPainter *painter, float size);
};

#endif // DRAGREGISTERINDICATOR_H
