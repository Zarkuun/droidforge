#ifndef ICONMARKER_H
#define ICONMARKER_H

#include <QGraphicsItem>

class IconMarker : public QGraphicsItem
{
    unsigned size;
    const QImage &image;

public:
    IconMarker(const QImage &image, unsigned size, QString toolTip);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
};


#endif // ICONMARKER_H
