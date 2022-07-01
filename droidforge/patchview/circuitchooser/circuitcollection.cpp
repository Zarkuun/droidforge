#include "circuitcollection.h"
#include "circuitinfoview.h"
#include "colorscheme.h"
#include "droidfirmware.h"
#include "tuning.h"
#include "globals.h"

#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QtGlobal>
#include <QResizeEvent>
#include <QScrollBar>

#define DATA_INDEX_INDEX 0

CircuitCollection::CircuitCollection(QString category, QWidget *parent)
    : QGraphicsView(parent)
    , numCircuits(0)
    , selectedCircuit(0)
{
    initScene();
    loadCircuitCategory(category);
    updateSceneRect(numCircuits);
    setFocusPolicy(Qt::NoFocus);
}
CircuitCollection::CircuitCollection(QWidget *parent)
    : QGraphicsView(parent)
    , numCircuits(0)
    , selectedCircuit(0)
{
    initScene();
}
CircuitCollection::~CircuitCollection()
{
}
void CircuitCollection::initScene()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(COLOR(CICH_COLOR_BACKGROUND));
    setScene(scene);
}
void CircuitCollection::updateSceneRect(int numCircuits)
{
    shoutfunc;
    QRect rect(
        0,  // x
        0,  // y
        CICH_GLOBAL_MARGIN * 2 + circuitViewWidth, // width
        2 * CICH_GLOBAL_MARGIN
        + (numCircuits - 1 ) * CICH_CIRCUIT_DISTANCE
        + numCircuits * CICH_CIRCUIT_HEIGHT);
    scene()->setSceneRect(rect);
}
void CircuitCollection::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        handleMousePress(event->pos());
    }
}
void CircuitCollection::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (handleMousePress(event->pos()))
        chooseCurrentCircuit();
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
    loadCircuitCategory("", text);
    selectedCircuit = qMin(selectedCircuit, circuits.size() - 1);
    updateSceneRect(numCircuits);
    update();
    setFocusPolicy(Qt::NoFocus);
}
void CircuitCollection::resizeEvent(QResizeEvent *event)
{
    circuitViewWidth = event->size().width() - CICH_WIDTH_MARGIN;
    QGraphicsView::resizeEvent(event);
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
bool CircuitCollection::handleMousePress(const QPoint &pos)
{
    for (auto item: items(pos)) {
        if (item->data(DATA_INDEX_INDEX).isValid()) {
            int index = item->data(DATA_INDEX_INDEX).toInt();
            if (currentCircuit())
                currentCircuit()->deselect();
            selectedCircuit = index;
            currentCircuit()->select();
            return true;
        }
    }
    return false;
}
void CircuitCollection::loadCircuitCategory(QString category, QString search)
{
    search = search.toLower();
    numCircuits = 0;

    unsigned y = CICH_GLOBAL_MARGIN;
    QStringList circuitNames = the_firmware->circuitsOfCategory(category);
    int index = 0;
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
        civ->setData(DATA_INDEX_INDEX, index++);
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
