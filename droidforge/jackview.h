#ifndef JACKVIEW_H
#define JACKVIEW_H

#include "jackline.h"

#include <QGraphicsItem>

class JackView : public QGraphicsItem
{
    QString jack;
    bool isInput;
    unsigned arraySize;
    bool isSelected;
    int subjack;
    JackLine *jackLine;

public:
    JackView(QString circuit, QString jack, bool isInput);
    QString getJack() const { return jack; };
    QRectF boundingRect() const override;
    bool isArray() const { return arraySize > 0; };
    unsigned getArraySize() const { return arraySize; };
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void setJackLine(JackLine *jl) { jackLine = jl; };
    void select(int subjack);
    void deselect();
};

#endif // JACKVIEW_H
