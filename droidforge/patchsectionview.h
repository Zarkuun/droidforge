#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "patchsection.h"
#include "tuning.h"

#include <QGraphicsView>

class PatchSectionView : public QGraphicsView
{
    PatchSection *section;
    QList<CircuitView *>circuitViews;

    // Cursor position
    int currentCircuitNr;
    int currentJack;
    int currentColumn;

public:
    PatchSectionView(PatchSection *section)
        : section(section)
        , currentCircuitNr(0)
        , currentJack(-1) // HEAD
        , currentColumn(0)
    {
        QGraphicsScene *scene = new QGraphicsScene();
        scene->setBackgroundBrush(COLOR_PATCH_BACKGROUND);
        setAlignment(Qt::AlignLeft | Qt::AlignTop);
        setScene(scene);
        buildPatchSection();
    }
    void handleKeyPress(int key);

private:
    void buildPatchSection();
    CircuitView *currentCircuitView();
    void moveCursorUpDown(int whence);
    void moveCursorLeftRight(int whence);
    void moveCursorPageUpDown(int whence);
};

#endif // PATCHSECTIONVIEW_H
