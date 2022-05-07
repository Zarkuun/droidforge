#include "jackselector.h"
#include "tuning.h"
#include "jackview.h"

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
    QStringList inputs = the_firmware->inputsOfCircuit(circuit);
    QStringList outputs = the_firmware->outputsOfCircuit(circuit);
    int maxNum = qMax(inputs.count(), outputs.count());
    int height = maxNum * JSEL_JACK_HEIGHT +
                 (maxNum-1) * JSEL_JACK_SPACING;

    placeJacks(inputs, height, 0);
    placeJacks(outputs, height, 1);

    if (jackViews[currentColumn].count() == 0)
        currentColumn = (currentColumn + 1) % 2;
    jackViews[currentColumn][0]->select();
}


void JackSelector::placeJacks(const QStringList &jacks, int height, int column)
{
    // QStringList outputs = the_firmware->inputsOfCircuit(circuit);
    int count = jacks.count();
    int spacing = 0;
    if (count > 1)
        spacing = (height - (count * JSEL_JACK_HEIGHT)) / (count - 1);
    qDebug() << "spacing" << spacing;

    int y = 0;
    int x = column * 400;
    for (qsizetype i=0; i<count; i++)
    {
        QString jack = jacks[i];
        JackView *jv = new JackView(jack, column == 0);
        scene()->addItem(jv);
        jv->setPos(x, y);
        y += spacing + JSEL_JACK_HEIGHT;
        jackViews[column].append(jv);
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
