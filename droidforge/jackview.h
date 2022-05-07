#ifndef JACKVIEW_H
#define JACKVIEW_H

#include <QGraphicsItem>

class JackView : public QGraphicsItem
{
    QString jack;
    bool isInput;
    unsigned arraySize;
    bool isSelected;
    int subjack;

public:
    JackView(QString circuit, QString jack, bool isInput);
    QString getJack() const { return jack; };
    QRectF boundingRect() const override;
    bool isArray() const { return arraySize > 0; };
    unsigned getArraySize() const { return arraySize; };
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void select(int subjack);
    void deselect();
};

#endif // JACKVIEW_H
