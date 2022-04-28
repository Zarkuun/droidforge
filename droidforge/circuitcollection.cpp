#include "circuitcollection.h"
#include "circuitinfoview.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QMouseEvent>
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
    backgroundRect = new QGraphicsRectItem(
        0,  // x
        0,  // y
        CICH_GLOBAL_MARGIN * 2 + CICH_CIRCUIT_WIDTH, // width
        2 * CICH_GLOBAL_MARGIN
        + (numCircuits - 1 ) * CICH_CIRCUIT_DISTANCE
        + numCircuits * CICH_CIRCUIT_HEIGHT);
    backgroundRect->setBrush(Qt::NoBrush);
    backgroundRect->setPen(Qt::NoPen);
    backgroundRect->setZValue(-1);
    scene->addItem(backgroundRect);
}


CircuitCollection::~CircuitCollection()
{
}

void CircuitCollection::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        if (!handleMousePress(event->pos())) {
            // NIX
        }
    }
}

bool CircuitCollection::handleMousePress(const QPointF &pos)
{
    qDebug() << "PRESS" << pos;
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());

    if (!item || item == backgroundRect)
        return false;

    CircuitInfoView *civ = (CircuitInfoView *)item;

    qDebug() << item << "NAME" << civ->getCircuit();
    emit selectCircuit(civ->getCircuit());
    return true;
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
