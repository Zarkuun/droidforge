#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{
public:
    CircuitCollection(QString category, QWidget *parent);
    ~CircuitCollection();

private:
    void loadCircuitCategory(QString category);
};

#endif // CIRCUITCOLLECTION_H
