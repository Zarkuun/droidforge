#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{

public:
    CircuitCollection(QWidget *parent);
    ~CircuitCollection();
};

#endif // CIRCUITCOLLECTION_H
