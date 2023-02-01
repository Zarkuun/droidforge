#ifndef JACKLINE_H
#define JACKLINE_H

#include <QGraphicsItem>

class JackLine : public QGraphicsItem
{
    QPoint start;
    QPoint end;
    float phase; // 0 ... 1
    bool active;
    bool isSelected;

public:
    JackLine(const QPoint &s, const QPoint &e, float p, bool active)
        : start(s), end(e), phase(p), active(active), isSelected(false) {};
    void select(bool s);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
};

#endif // JACKLINE_H
