#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{
    Q_OBJECT

    QGraphicsRectItem *backgroundRect;

public:
    CircuitCollection(QString category, QWidget *parent);
    ~CircuitCollection();
    void mousePressEvent(QMouseEvent *event);

private:
    bool handleMousePress(const QPointF &pos);
    unsigned loadCircuitCategory(QString category);

signals:
    void selectCircuit(QString name);
};

#endif // CIRCUITCOLLECTION_H
