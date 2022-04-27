#include "circuitcollection.h"

CircuitCollection::CircuitCollection(QWidget *parent)
    : QGraphicsView(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


CircuitCollection::~CircuitCollection()
{
}
