#include "circuitcollection.h"
#include "circuitinfoview.h"
#include "droidfirmware.h"

CircuitCollection::CircuitCollection(QString category, QWidget *parent)
    : QGraphicsView(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(QColor(40, 50, 60));
    setScene(scene);

    loadCircuitCategory(category);
}

CircuitCollection::~CircuitCollection()
{
}


void CircuitCollection::loadCircuitCategory(QString category)
{
    unsigned y = 0;
    QStringList circuits = the_firmware->circuitsOfCategory(category);
    for (qsizetype i=0; i<circuits.size(); i++) {
        QString circuit = circuits[i];
        QString description = the_firmware->circuitDescription(circuit);
        CircuitInfoView *civ = new CircuitInfoView(circuit, description);
        scene()->addItem(civ);
        civ->setPos(0, y);
        y += civ->boundingRect().height();
        qDebug() << "HAB" << circuit << description;
    }
}
