#include "circuitcollection.h"
#include "circuitinfoview.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QtGlobal>

CircuitCollection::CircuitCollection(QString category, QWidget *parent)
    : QGraphicsView(parent)
    , selectedCircuit(0)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(CICH_BACKGROUND_COLOR);

    setScene(scene);
    numCircuits = loadCircuitCategory(category);


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


void CircuitCollection::keyPressEvent(QKeyEvent *event)
{
    qDebug() << Q_FUNC_INFO << event;
    if (event->key() == Qt::Key_Down)
        moveCursorUpDown(1);
    else if (event->key() == Qt::Key_Up)
        moveCursorUpDown(-1);
    else
        QWidget::keyPressEvent(event);
}

QString CircuitCollection::selectedCircuitName()
{
    return currentCircuit()->getCircuit();
}


bool CircuitCollection::handleMousePress(const QPointF &pos)
{
    qDebug() << "PRESS" << pos;
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());

    if (!item || item == backgroundRect)
        return false;

    CircuitInfoView *civ = (CircuitInfoView *)item;

    // Find index of clicked circuit and select it
    currentCircuit()->deselect();
    for (qsizetype i=0; i<numCircuits; i++) {
        if (civ == circuits[i]) {
            selectedCircuit = i;
            currentCircuit()->select();
        }
    }

    // qDebug() << item << "NAME" << civ->getCircuit();
    // emit selectCircuit(civ->getCircuit());
    return true;
}


unsigned CircuitCollection::loadCircuitCategory(QString category)
{
    unsigned y = CICH_GLOBAL_MARGIN;
    QStringList circuitNames = the_firmware->circuitsOfCategory(category);
    for (qsizetype i=0; i<circuitNames.size(); i++) {
        QString circuit = circuitNames[i];
        QString description = the_firmware->circuitDescription(circuit);
        CircuitInfoView *civ = new CircuitInfoView(circuit, description);
        circuits.append(civ);
        if (i == selectedCircuit)
            civ->select();
        scene()->addItem(civ);
        civ->setPos(CICH_GLOBAL_MARGIN, y);
        y += civ->boundingRect().height() + CICH_CIRCUIT_DISTANCE;
    }
    return circuitNames.size();
}


void CircuitCollection::moveCursorUpDown(int whence)
{
    currentCircuit()->deselect();
    selectedCircuit = qMax(0, qMin(numCircuits-1, selectedCircuit + whence));
    currentCircuit()->select();
    ensureVisible(currentCircuit());
}


CircuitInfoView *CircuitCollection::currentCircuit()
{
    return circuits[selectedCircuit];
}


void CircuitCollection::chooseCurrentCircuit()
{
    QString name = currentCircuit()->getCircuit();
    qDebug() << Q_FUNC_INFO << currentCircuit() << name;
    emit selectCircuit();
}
