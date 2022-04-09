#include "patchsectionview.h"
#include "circuitview.h"
#include "tuning.h"

#define CIRCUIT_MARGIN 10



void PatchSectionView::handleKeyPress(int key)
{
    qDebug() << "KEY" << key;
    switch (key) {
        case Qt::Key_Up:
                moveCursorUpDown(-1);
                break;
        case Qt::Key_Down:
                moveCursorUpDown(1);
                break;
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
    currentCircuitView()->select(-1);
}


CircuitView *PatchSectionView::currentCircuitView()
{
    return circuitViews[currentCircuitNr];
}


void PatchSectionView::moveCursorUpDown(int whence)
{
    currentCircuitView()->deselect();
    currentCircuitView()->update();

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

    currentCircuitView()->select(currentJack);
    currentCircuitView()->update();
    centerOn(currentCircuitView());
}
