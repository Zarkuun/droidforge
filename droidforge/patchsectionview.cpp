#include "patchsectionview.h"
#include "circuitview.h"
#include "tuning.h"

#define CIRCUIT_MARGIN 10

PatchSectionView::PatchSectionView(PatchSection *section)
    : section(section)
{
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(PATCH_BACKGROUND_COLOR);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(scene);
    buildPatchSection();
}


void PatchSectionView::buildPatchSection()
{
    unsigned y = 0;
    for (qsizetype i=0; i<section->circuits.size(); i++)
    {
        Circuit *circuit = &section->circuits[i];
        CircuitView *cv = new CircuitView(circuit);
        scene()->addItem(cv);
        cv->setPos(0, y);
        y += cv->boundingRect().height() + CIRCUIT_MARGIN;
    }
}
