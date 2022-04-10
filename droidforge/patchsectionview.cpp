#include "patchsectionview.h"
#include "circuitview.h"
#include "tuning.h"

#define CIRCUIT_MARGIN 10



void PatchSectionView::handleKeyPress(int key)
{
    qDebug() << "KEY" << key;
    switch (key) {
    case Qt::Key_Up:       moveCursorUpDown(-1); break;
    case Qt::Key_Down:     moveCursorUpDown(1);  break;
    case Qt::Key_Left:     moveCursorLeftRight(-1); break;
    case Qt::Key_Right:    moveCursorLeftRight(1);  break;
    case Qt::Key_PageUp:   moveCursorPageUpDown(-1); break;
    case Qt::Key_PageDown: moveCursorPageUpDown(1); break;
    }
}


void PatchSectionView::buildPatchSection()
{
    unsigned y = 0;
    for (qsizetype i=0; i<section->circuits.size(); i++)
    {
        Circuit *circuit = section->circuits[i];
        CircuitView *cv = new CircuitView(circuit);
        circuitViews.append(cv);
        scene()->addItem(cv);
        cv->setPos(0, y);
        y += cv->boundingRect().height() + CIRCUIT_MARGIN;
    }
    currentCircuitView()->select(-1, currentColumn);
}


CircuitView *PatchSectionView::currentCircuitView()
{
    return circuitViews[currentCircuitNr];
}


void PatchSectionView::moveCursorLeftRight(int whence)
{
    if (whence == -1 && currentColumn == 0)
        return;

    else if (whence == 1 && currentColumn >= 3)
        return;

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
    centerOn(currentCircuitView());
}


void PatchSectionView::moveCursorUpDown(int whence)
{
    currentCircuitView()->deselect();

    if (whence == 1) // down
    {
        unsigned n = currentCircuitView()->numJackAssignments();
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
    centerOn(currentCircuitView());
}
