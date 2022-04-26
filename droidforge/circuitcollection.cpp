#include "circuitcollection.h"

CircuitCollection::CircuitCollection(const QString &title, QWidget *parent)
    : QGraphicsView(parent)
    , title(title)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


CircuitCollection::~CircuitCollection()
{
}
