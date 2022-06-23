#include "jackselector.h"
#include "colorscheme.h"
#include "globals.h"
#include "jackcircuitview.h"
#include "tuning.h"
#include "jackview.h"
#include "jackline.h"

#include <QKeyEvent>
#include <QtGlobal>

JackSelector::JackSelector(QWidget *parent)
    : QGraphicsView(parent)
    , currentRow(0)
    , currentColumn(0)
    , currentSubjack(0)
    , usedJacks(0)
    , jackType(JACKTYPE_DONT_CARE)
{
    initScene();
    setMinimumWidth(700);
}
void JackSelector::setCircuit(const QString &c, const QString &current, const QStringList &uj, jacktype_t onlyType, QString search)
{
    circuit = c;
    usedJacks = &uj;
    jackType = onlyType;
    loadJacks(circuit, search);
    setCursor(current);
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
    else if (event->key() == Qt::Key_Home)
        moveCursorHomeEnd(-1);
    else if (event->key() == Qt::Key_End)
        moveCursorHomeEnd(1);
    else
        QWidget::keyPressEvent(event);
}
void JackSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        handleMousePress(event->pos());
}
void JackSelector::mouseDoubleClickEvent(QMouseEvent *event)
{
    handleMousePress(event->pos());
    emit accepted();
}
QString JackSelector::getSelectedJack() const
{
    QString jack = currentJack()->getJack();
    if (currentJack()->isArray())
        jack += QString::number(currentSubjack+1);
    return jack;
}
void JackSelector::initScene()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(QBrush(the_colorscheme->backgroundPixmap()));
    setScene(scene);
}
void JackSelector::loadJacks(QString circuit, QString search)
{
    initScene();
    // scene()->clear();
    for (int i=0; i<2; i++)
        jackViews[i].clear();

    // Add jacks
    QStringList inputs = the_firmware->jackGroupsOfCircuit(circuit, "inputs", search);
    QStringList outputs = the_firmware->jackGroupsOfCircuit(circuit, "outputs", search);

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
    ensureVisible(jcv);

    scene()->addRect(0, -JSEL_VERTICAL_PADDING, 0, totalHeight+2*JSEL_VERTICAL_PADDING, Qt::NoPen, Qt::NoBrush);
}
void JackSelector::setCursor(QString current)
{
    if (!current.isEmpty()) {
        bool found=false;
        for (unsigned c=0; c<2; c++) {
            for (qsizetype i=0; i<jackViews[c].count(); i++) {
                if (jackViews[c][i]->getJack() == current) {
                    currentColumn = c;
                    currentRow = i;
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
    }
    if (jackViews[currentColumn].count() == 0)
        currentColumn = (currentColumn + 1) % 2;
    currentRow = qMin(currentRow, jackViews[currentColumn].count()-1);
    selectCurrentJack(true);
}
unsigned JackSelector::createJacks(const QStringList &jacks, int column)
{
    unsigned height = 0;
    for (qsizetype i=0; i<jacks.count(); i++) {
        QString jack = jacks[i];
        JackView *jv = new JackView(circuit, jack, usedJacks, jackType, column == 0);
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

        JackLine *jl = new JackLine(QPoint(xa, ya), QPoint(xo, yo), phase + 0.25, jv->isActive(-1));
        jackViews[column][i]->setJackLine(jl);
        scene()->addItem(jl);
        yo += linespacePerJack;
        y += spacePerJack + h;
    }
}
void JackSelector::moveCursorUpDown(int whence)
{
    int rows = jackViews[currentColumn].count();
    bool canGoDown = currentRow < rows-1;
    bool canGoUp = currentRow > 0;

    JackView *jv = currentJack();
    if (jv->isArray()) {
        if (whence == -1 && currentSubjack < 4 && !canGoUp)
            return;
        if (whence == 1 && currentSubjack + 4 >= (int)jv->getArraySize() && !canGoDown)
            return;
        currentSubjack += 4 * whence;
        if (currentSubjack >= 0 && currentSubjack < (int)jv->getArraySize()) {
            selectCurrentJack(true);
            return;
        }
    }

    if (whence == 1 && !canGoDown)
        return;
    if (whence == -1 && !canGoUp)
        return;

    selectCurrentJack(false);
    currentRow += whence;
    jv = currentJack();
    if (jv->isArray() && whence == -1) {
        currentSubjack = jv->getArraySize()-4;
    }
    else
        currentSubjack = 0;

    selectCurrentJack(true);
    ensureVisible(currentJack(), JSEL_SCROLL_MARGIN, JSEL_SCROLL_MARGIN);
    scene()->update();
}

void JackSelector::moveCursorHomeEnd(int whence)
{
    selectCurrentJack(false);

    if (whence == -1)
        currentRow = 0;

    else {
        int rows = jackViews[currentColumn].count();
        currentRow = rows - 1;
    }

    currentSubjack = 0;
    selectCurrentJack(true);
    ensureVisible(currentJack(), JSEL_SCROLL_MARGIN, JSEL_SCROLL_MARGIN);
    scene()->update();
}
void JackSelector::moveCursorLeftRight(int whence)
{
    JackView *jv = currentJack();
    if (jv->isArray()) {
        int col = currentSubjack % 4;
        int width = qMin(4, (int)jv->getArraySize());
        if ((whence == -1 && col > 0) || (whence == 1 && col < (width-1)))
        {
            currentSubjack += whence;
            selectCurrentJack(true);
            return;
         }
    }

    if (whence == -1 && currentColumn == 0)
        return;
    else if (whence == 1 && currentColumn == 1)
        return;

    selectCurrentJack(false);
    currentSubjack = 0;

    int count = jackViews[currentColumn].count();
    float relpos = float(currentRow) / (count - 1);
    if (whence == -1)
        currentColumn = 0;
    else
        currentColumn = 1;
    count = jackViews[currentColumn].count();
    currentRow = round(relpos * (count - 1));
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
        jv->select(currentSubjack);
    else
        jv->deselect();

    QString jackName = jv->getJack();
    if (jv->isArray())
        jackName += QString::number(currentSubjack + 1);

    emit cursorMoved(
                jackName,
                currentColumn == 0 ? JACKTYPE_INPUT : JACKTYPE_OUTPUT,
                jv->isActive(currentSubjack));
}
bool JackSelector::handleMousePress(const QPointF &pos)
{
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());
    if (!item)
        return false;

    // Find jack view. We have other (useless) items as well
    for (int column = 0; column < 2; column++) {
        auto jvs = &jackViews[column];
        for (int i=0; i<jvs->count(); i++) {
            JackView *jv = (*jvs)[i];
            if (jv == item) {
                selectCurrentJack(false);
                currentColumn = column;
                currentRow = i;
                if (jv->isArray()) {
                    QPointF mousePosInScene = mapToScene(pos.toPoint());
                    QPointF relPos = mousePosInScene - jv->pos();
                    currentSubjack = 0;
                    if (relPos.y() >= JSEL_JACK_HEIGHT) {
                        int y = (relPos.y() - JSEL_JACK_HEIGHT) / JSEL_JACK_HEIGHT;
                        int x = relPos.x() / (JSEL_JACK_WIDTH / 4.0);
                        currentSubjack = qMin((int)jv->getArraySize(), qMax(0, y * 4 + x));
                    }
                }
                else
                    currentSubjack = 0;

                selectCurrentJack(true);
                return true;
            }
        }
    }
    return false;
}
void JackSelector::searchChanged(QString text)
{
    loadJacks(circuit, text);
    setCursor();
}
