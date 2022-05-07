#ifndef JACKVIEW_H
#define JACKVIEW_H

#include <QGraphicsItem>

class JackView : public QGraphicsItem
{
    QString jack;
    bool isInput;
    bool isSelected;

public:
    JackView(QString jack, bool isInput);
    QString getJack() const { return jack; };
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void select();
    void deselect();
};

#endif // JACKVIEW_H
