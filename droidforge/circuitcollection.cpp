#include "circuitcollection.h"
#include "circuitinfoview.h"
#include "droidfirmware.h"
#include "tuning.h"


#include <QGraphicsRectItem>

CircuitCollection::CircuitCollection(QString category, QWidget *parent)
    : QGraphicsView(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(CICH_BACKGROUND_COLOR);


    setScene(scene);
    unsigned numCircuits = loadCircuitCategory(category);


    // Create an invisible rectangle that works as a global
    // bounding box. That will make sure that the visible area
    // has the correct margins.
    QGraphicsRectItem *rect = new QGraphicsRectItem(
        0,  // x
        0,  // y
        CICH_GLOBAL_MARGIN * 2 + CICH_CIRCUIT_WIDTH, // width
        2 * CICH_GLOBAL_MARGIN
        + (numCircuits - 1 ) * CICH_CIRCUIT_DISTANCE
        + numCircuits * CICH_CIRCUIT_HEIGHT);
    rect->setBrush(Qt::NoBrush);
    rect->setPen(Qt::NoPen);
    scene->addItem(rect);
}


CircuitCollection::~CircuitCollection()
{
}


unsigned CircuitCollection::loadCircuitCategory(QString category)
{
    unsigned y = CICH_GLOBAL_MARGIN;
    QStringList circuits = the_firmware->circuitsOfCategory(category);
    for (qsizetype i=0; i<circuits.size(); i++) {
        QString circuit = circuits[i];
        QString description = the_firmware->circuitDescription(circuit);
        CircuitInfoView *civ = new CircuitInfoView(circuit, description);
        scene()->addItem(civ);
        civ->setPos(CICH_GLOBAL_MARGIN, y);
        y += civ->boundingRect().height() + CICH_CIRCUIT_DISTANCE;
    }
    return circuits.size();
}
