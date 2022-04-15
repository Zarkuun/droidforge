#include "patchsectionview.h"
#include "circuitview.h"
#include "droidforge.h"
#include "tuning.h"

#include <QMouseEvent>

PatchSectionView::PatchSectionView(PatchSection *section)
    : section(section)
    , currentCircuitNr(0)
    , currentJack(-1) // HEAD
    , currentColumn(0)
{
    setAlignment(Qt::AlignCenter | Qt::AlignTop);
    buildPatchSection();
}


void PatchSectionView::buildPatchSection()
{
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(COLOR_PATCH_BACKGROUND);
    setScene(scene);

    unsigned y = CIRCUIT_VERTICAL_MARGIN;
    for (qsizetype i=0; i<section->circuits.size(); i++)
    {
        Circuit *circuit = section->circuits[i];
        CircuitView *cv = new CircuitView(circuit);
        circuitViews.append(cv);
        scene->addItem(cv);
        cv->setPos(0, y); // TODO: der erste parameter wirkt nicht
        y += cv->boundingRect().height();
    }
    currentCircuitView()->select(-1, currentColumn);
}

void PatchSectionView::deletePatchSection()
{
    for (unsigned i=0; i<circuitViews.size(); i++)
        delete circuitViews[i];
    circuitViews.clear();
}


void PatchSectionView::rebuildPatchSection()
{
    deletePatchSection();
    buildPatchSection();
}


bool PatchSectionView::handleKeyPress(int key)
{
    qDebug() << "KEY" << key;
    switch (key) {
    case Qt::Key_Up:       moveCursorUpDown(-1);     return true;
    case Qt::Key_Down:     moveCursorUpDown(1);      return true;
    case Qt::Key_Left:     moveCursorLeftRight(-1);  return true;
    case Qt::Key_Right:    moveCursorLeftRight(1);   return true;
    case Qt::Key_PageUp:   moveCursorPageUpDown(-1); return true;
    case Qt::Key_PageDown: moveCursorPageUpDown(1);  return true;
    case Qt::Key_Backspace: deleteCurrentRow();      return true;
    default: return false;
    }
}


void PatchSectionView::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "MAUS" << event;
    if (event->type() == QMouseEvent::MouseButtonPress) {
        if (!handleMousePress(event->pos())) {
            qDebug() << "Unhandled mouse press" << event;
        }
    }
}


bool PatchSectionView::handleMousePress(const QPointF &pos)
{
    // pos are the coordinates relative to the window where
    // the patch is being displayed.
    qDebug() << "Mause bei " << pos.x() << pos.y();

    // itemAt() applies the transformation of the graphics
    // view such as the scroll bar and the alignment.
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());

    if (!item)
        return false;


    CircuitView *cv = (CircuitView *)item;
    for (unsigned i=0; i<circuitViews.size(); i++)
    {
        // Now we need the coordinates relative to the circuitview
        // itself.
        QPointF posInScene(mapToScene(pos.toPoint()));
        QPointF posInCircuit = posInScene - item->pos();

        if (circuitViews[i] == cv) {
            currentCircuitView()->deselect();
            currentCircuitNr = i;
            currentJack = cv->jackAt(posInCircuit.y());
            currentColumn = cv->columnAt(posInCircuit.x());
            currentCircuitView()->select(currentJack, currentColumn);
            ensureVisible(currentCircuitView());
        }
    }
    return true;
}


CircuitView *PatchSectionView::currentCircuitView()
{
    return circuitViews[currentCircuitNr];
}


Circuit *PatchSectionView::currentCircuit()
{
    return section->circuits[currentCircuitNr];

}


void PatchSectionView::moveCursorLeftRight(int whence)
{
    if (whence == -1 && currentColumn == 0)
        return;

    else if (whence == 1 && currentColumn >= 3)
        return;

    if (whence == -1
        && currentJack >= 0
        && section->circuits[currentCircuitNr]->jackAssignment(currentJack)->jackType != JACKTYPE_INPUT
        && currentColumn <= 3)
    {
        currentColumn = 0;
    }
    else
        currentColumn += whence;
    currentCircuitView()->select(currentJack, currentColumn);
}

void PatchSectionView::moveCursorPageUpDown(int whence)
{
    if (currentJack == -1) {
        currentCircuitView()->deselect();
        currentCircuitNr += whence;
        if (currentCircuitNr < 0)
            currentCircuitNr = 0;
        else if (currentCircuitNr >= circuitViews.size())
            currentCircuitNr = circuitViews.size() - 1;
    }
    else
        currentJack = -1;
    currentCircuitView()->select(currentJack, currentColumn);
    ensureVisible(currentCircuitView());
}


void PatchSectionView::deleteCurrentRow()
{
    if (currentJack == -1)
        deleteCurrentCircuit();
    else
        deleteCurrentJack();
}


void PatchSectionView::deleteCurrentCircuit()
{
    the_forge->registerEdit("Delete circuit");
    section->deleteCircuitNr(currentCircuitNr);
    if (currentCircuitNr >= section->circuits.count())
        currentCircuitNr--;
    currentColumn = 0;
    currentJack = -1;
    rebuildPatchSection();
}


void PatchSectionView::deleteCurrentJack()
{
    the_forge->registerEdit("Delete jack assignment");
    Circuit *circuit = currentCircuit();
    circuit->deleteJackAssignment(currentJack);
    if (currentJack >= circuit->numJackAssignments())
        currentJack --;
    rebuildPatchSection();
    currentCircuitView()->select(currentJack, currentColumn);
}


void PatchSectionView::moveCursorUpDown(int whence)
{
    currentCircuitView()->deselect();

    if (whence == 1) // dowldiln
    {
        int n = currentCircuitView()->numJackAssignments();
        currentJack ++;
        if (currentJack >= n) {
            currentCircuitNr ++;
            if (currentCircuitNr >= circuitViews.size()) {
                currentJack--;
                currentCircuitNr--;
            }
            else
                currentJack = -1;
        }
    }
    else // up
    {
        currentJack --;
        if (currentJack < -1) {
            currentCircuitNr--;
            if (currentCircuitNr < 0) {
                currentCircuitNr = 0;
                currentJack = -1;
            }
            else
                currentJack = currentCircuitView()->numJackAssignments() - 1;
        }

    }

    currentCircuitView()->select(currentJack, currentColumn);
    ensureVisible(currentCircuitView());
}
