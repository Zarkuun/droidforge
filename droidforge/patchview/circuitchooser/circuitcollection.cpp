#include "circuitcollection.h"
#include "circuitinfoview.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QtGlobal>
#include <QResizeEvent>
#include <QScrollBar>

CircuitCollection::CircuitCollection(QString category, QWidget *parent)
    : QGraphicsView(parent)
    , backgroundRect(0)
    , selectedCircuit(0)
{
    initScene();
    loadCircuitCategory(category);
    initBackgroundRect(numCircuits);
}

CircuitCollection::CircuitCollection(QWidget *parent)
    : QGraphicsView(parent)
    , backgroundRect(0)
    , selectedCircuit(0)
    , numCircuits(0)
{
    initScene();
}

void CircuitCollection::initScene()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(CICH_BACKGROUND_COLOR);
    setScene(scene);
}

void CircuitCollection::initBackgroundRect(int numCircuits)
{
    // Create an invisible rectangle that works as a global
    // bounding box. That will make sure that the visible area
    // has the correct margins.
    if (backgroundRect) {
        scene()->removeItem(backgroundRect);
        delete backgroundRect;
   }

    backgroundRect = new QGraphicsRectItem(
        0,  // x
        0,  // y
        CICH_GLOBAL_MARGIN * 2 + circuitViewWidth, // width
        2 * CICH_GLOBAL_MARGIN
        + (numCircuits - 1 ) * CICH_CIRCUIT_DISTANCE
        + numCircuits * CICH_CIRCUIT_HEIGHT);
    backgroundRect->setBrush(Qt::NoBrush);
    backgroundRect->setPen(Qt::NoPen);
    backgroundRect->setZValue(-1);
    scene()->addItem(backgroundRect);
}

CircuitCollection::~CircuitCollection()
{
}

void CircuitCollection::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        if (!handleMousePress(event->pos())) {
            // TODO
        }
    }
}

void CircuitCollection::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (handleMousePress(event->pos()))
            chooseCurrentCircuit();
}

void CircuitCollection::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down)
        moveCursorUpDown(1);
    else if (event->key() == Qt::Key_Up)
        moveCursorUpDown(-1);
    else
        QWidget::keyPressEvent(event);
}

QString CircuitCollection::selectedCircuitName()
{
    CircuitInfoView *civ = currentCircuit();
    if (civ)
        return civ->getCircuit();
    else
        return ""; // empty search
}

void CircuitCollection::updateSearch(QString text)
{
    scene()->clear();
    circuits.clear();
    backgroundRect = 0;
    loadCircuitCategory("", text);
    selectedCircuit = qMin(selectedCircuit, circuits.size() - 1);
    initBackgroundRect(numCircuits);
    update();
}

void CircuitCollection::resizeEvent(QResizeEvent *event)
{
    circuitViewWidth = event->size().width() - CICH_WIDTH_MARGIN;
}

bool CircuitCollection::preselectCircuit(QString name)
{
    for (qsizetype i=0; i<numCircuits; i++) {
        CircuitInfoView *civ = circuits[i];
        if (civ->getCircuit() == name) {
            if (currentCircuit()) {
                currentCircuit()->deselect();
                selectedCircuit = i;
                currentCircuit()->select();
                return true;
            }
        }
    }
    return false;
}

bool CircuitCollection::handleMousePress(const QPointF &pos)
{
    QGraphicsItem *item = itemAt(pos.x(), pos.y());

    if (!item || item == backgroundRect)
        return false;

    CircuitInfoView *civ = (CircuitInfoView *)item;

    // Find index of clicked circuit and select it
    if (currentCircuit())
        currentCircuit()->deselect();
    for (qsizetype i=0; i<numCircuits; i++) {
        if (civ == circuits[i]) {
            selectedCircuit = i;
            currentCircuit()->select();
        }
    }
    return true;
}

void CircuitCollection::loadCircuitCategory(QString category, QString search)
{
    search = search.toLower();
    numCircuits = 0;

    unsigned y = CICH_GLOBAL_MARGIN;
    QStringList circuitNames = the_firmware->circuitsOfCategory(category);
    for (qsizetype i=0; i<circuitNames.size(); i++) {
        QString circuit = circuitNames[i];
        QString description = the_firmware->circuitDescription(circuit);
        if (!search.isEmpty()
            && !circuit.contains(search, Qt::CaseInsensitive))
            // && !description.contains(search, Qt::CaseInsensitive))
        {
            continue;
        }
        CircuitInfoView *civ = new CircuitInfoView(circuit, description, &circuitViewWidth);
        circuits.append(civ);
        numCircuits ++;
        if (i == selectedCircuit)
            civ->select();
        scene()->addItem(civ);
        civ->setPos(CICH_GLOBAL_MARGIN, y);
        y += civ->boundingRect().height() + CICH_CIRCUIT_DISTANCE;
    }
    moveCursorUpDown(0); // sanitize cursor position
}

void CircuitCollection::moveCursorUpDown(int whence)
{
    if (circuits.empty())
        return;

    currentCircuit()->deselect();
    selectedCircuit = qMax(0, qMin(numCircuits-1, selectedCircuit + whence));
    currentCircuit()->select();
    ensureVisible(currentCircuit(), 0, STANDARD_SPACING);
}

CircuitInfoView *CircuitCollection::currentCircuit()
{
    if (circuits.empty())
        return 0;
    else
        return circuits[qMax(0, qMin(selectedCircuit, circuits.size()-1))];
}

void CircuitCollection::chooseCurrentCircuit()
{
    emit selectCircuit();
}
