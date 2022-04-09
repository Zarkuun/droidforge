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
    currentCircuitView()->select();
}


CircuitView *PatchSectionView::currentCircuitView()
{
    return circuitViews[currentCircuitNr];
}


void PatchSectionView::moveCursorUpDown(int whence)
{
    currentCircuitView()->deselect();
    currentCircuitView()->update();

    currentCircuitNr += whence;
    if (currentCircuitNr < 0)
        currentCircuitNr = 0;
    else if (currentCircuitNr >= circuitViews.size())
        currentCircuitNr = circuitViews.size() - 1;
    currentCircuitView()->select();
    currentCircuitView()->update();
    qDebug() << currentCircuitNr;
}
