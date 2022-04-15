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
    PatchSectionView(PatchSection *section);
    ~PatchSectionView() { deletePatchSection(); };
    bool handleKeyPress(int key);
    void mousePressEvent(QMouseEvent *event);
    bool handleMousePress(int x, int y);

private:
    void buildPatchSection();
    void deletePatchSection();
    void rebuildPatchSection();
    CircuitView *currentCircuitView();
    Circuit *currentCircuit();
    void moveCursorUpDown(int whence);
    void moveCursorLeftRight(int whence);
    void moveCursorPageUpDown(int whence);
    void deleteCurrentRow();
    void deleteCurrentCircuit();
    void deleteCurrentJack();
};

#endif // PATCHSECTIONVIEW_H
