#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{
    QGraphicsRectItem *backgroundRect;

public:
    CircuitCollection(QString category, QWidget *parent);
    ~CircuitCollection();
    void mousePressEvent(QMouseEvent *event);

private:
    bool handleMousePress(const QPointF &pos);
    unsigned loadCircuitCategory(QString category);

};

#endif // CIRCUITCOLLECTION_H
