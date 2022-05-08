#ifndef JACKVIEW_H
#define JACKVIEW_H

#include "jackline.h"
#include "tuning.h"

#include <QGraphicsItem>

class JackView : public QGraphicsItem
{
    QString jack;
    bool isInput;
    unsigned arraySize;
    bool active;
    bool isSelected;
    int subjack;
    JackLine *jackLine;
    bool activeSubjacks[MAX_NUM_SUBJACKS];

public:
    JackView(QString circuit, QString jack, const QStringList &usedJacks, bool isInput);
    QString getJack() const { return jack; };
    QRectF boundingRect() const override;
    bool isActive(int subJack) const;
    bool isArray() const { return arraySize > 0; };
    unsigned getArraySize() const { return arraySize; };
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    void setJackLine(JackLine *jl) { jackLine = jl; };
    void select(int subjack);
    void deselect();
};

#endif // JACKVIEW_H
