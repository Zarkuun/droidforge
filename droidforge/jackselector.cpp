#include "jackselector.h"
#include "jackcircuitview.h"
#include "tuning.h"
#include "jackview.h"
#include "jackline.h"

#include <QKeyEvent>

JackSelector::JackSelector(QWidget *parent)
    : QGraphicsView(parent)
    , currentRow(0)
    , currentColumn(0)
{
    initScene();
}


void JackSelector::setCircuit(const QString &c)
{
    circuit = c;
    loadJacks(circuit, "");
}

void JackSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down)
        moveCursorUpDown(1);
    else if (event->key() == Qt::Key_Up)
        moveCursorUpDown(-1);
    else if (event->key() == Qt::Key_Left)
        moveCursorLeftRight(-1);
    else if (event->key() == Qt::Key_Right)
        moveCursorLeftRight(1);
    else
        QWidget::keyPressEvent(event);
}

QString JackSelector::getSelectedJack() const
{
    return currentJack()->getJack();

}


void JackSelector::initScene()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    QGraphicsScene *scene = new QGraphicsScene();
    QPixmap background(":images/background.png");
    scene->setBackgroundBrush(QBrush(background.scaledToHeight(JSEL_BACKGROUND_HEIGHT)));
    setScene(scene);
}

void JackSelector::loadJacks(QString circuit, QString)
{
    scene()->clear();
    for (int i=0; i<2; i++)
        jackViews[i].clear();

    // Add jacks
    QStringList inputs = the_firmware->jackGroupsOfCircuit(circuit, "inputs");
    QStringList outputs = the_firmware->jackGroupsOfCircuit(circuit, "outputs");

    int nettoInputHeight = createJacks(inputs, 0);
    int nettoOutputHeight = createJacks(outputs, 1);

    int bruttoInputHeight = inputs.count() > 0
            ? nettoInputHeight + (inputs.count() - 1) * JSEL_JACK_SPACING
            : 0;
    int bruttoOutputHeight = outputs.count() > 0
            ? nettoOutputHeight + (outputs.count() - 1) * JSEL_JACK_SPACING
            : 0;

    int totalHeight = qMax(bruttoInputHeight, bruttoOutputHeight);

    placeJacks(totalHeight, totalHeight - nettoInputHeight, 0);
    placeJacks(totalHeight, totalHeight - nettoOutputHeight, 1);

    // Put Icon with Circuit in the center
    JackCircuitView *jcv = new JackCircuitView(circuit);
    int x = (JSEL_TOTAL_WIDTH - JSEL_CIRCUIT_WIDTH) / 2;
    int y = (totalHeight - JSEL_CIRCUIT_HEIGHT) / 2;
    scene()->addItem(jcv);
    jcv->setPos(x, y);

    if (jackViews[currentColumn].count() == 0)
        currentColumn = (currentColumn + 1) % 2;
    jackViews[currentColumn][0]->select();
}


unsigned JackSelector::createJacks(const QStringList &jacks, int column)
{
    unsigned height = 0;
    for (qsizetype i=0; i<jacks.count(); i++) {
        QString jack = jacks[i];
        JackView *jv = new JackView(circuit, jack, column == 0);
        jackViews[column].append(jv);
        scene()->addItem(jv);
        height += jv->boundingRect().height();
    }
    return height;
}


void JackSelector::placeJacks(int totalHeight, float space, int column)
{
    QList<JackView *> *jvs = &jackViews[column];
    if (!jvs->count())
        return;

    float spacePerJack = jvs->count() > 1
              ? space / (jvs->count() - 1)
              : 0;

    float linespacePerJack = JSEL_CIRCUIT_HEIGHT / jvs->count();

    int rightColumn = JSEL_TOTAL_WIDTH - JSEL_JACK_WIDTH;
    int x = column * rightColumn;
    int y = 0;

    int yo = (totalHeight - JSEL_CIRCUIT_HEIGHT) / 2 + linespacePerJack / 2;

    for (qsizetype i=0; i<jvs->count(); i++)
    {
        JackView *jv = (*jvs)[i];
        jv->setPos(x, y);
        unsigned h = jv->boundingRect().height();
        // Create line from jack to center
        unsigned xa = column == 0 ? JSEL_JACK_WIDTH : rightColumn;
        unsigned xo = JSEL_TOTAL_WIDTH / 2;
        unsigned ya = y + h/2;
        if (column == 0)
            xo -= JSEL_CIRCUIT_WIDTH / 2;
        else
            xo += JSEL_CIRCUIT_WIDTH / 2;

        float phase;
        if (i < jvs->count() / 2)
            phase = float(i + 0.5) / jvs->count();
        else
            phase = 1.0 - (float(i + 0.5) / jvs->count());

        JackLine *jl = new JackLine(QPoint(xa, ya), QPoint(xo, yo), phase + 0.25);
        scene()->addItem(jl);
        yo += linespacePerJack;
        y += spacePerJack + h;
    }
}


void JackSelector::moveCursorUpDown(int whence)
{
    int rows = jackViews[currentColumn].count();
    selectCurrentJack(false);
    currentRow = qMin(rows-1, qMax(0, currentRow + whence));
    selectCurrentJack(true);
}


void JackSelector::moveCursorLeftRight(int whence)
{
    if (whence == -1 && currentColumn == 0)
        return;
    else if (whence == 1 && currentColumn == 1)
        return;

    selectCurrentJack(false);
    if (whence == -1)
        currentColumn = 0;
    else
        currentColumn = 1;
    selectCurrentJack(true);
}

JackView *JackSelector::currentJack()
{
    return jackViews[currentColumn][currentRow];
}

const JackView *JackSelector::currentJack() const
{
    return jackViews[currentColumn][currentRow];
}

void JackSelector::selectCurrentJack(bool sel)
{
    JackView *jv = currentJack();
    if (sel)
        jv->select();
    else
        jv->deselect();
}
