#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "patchsection.h"
#include "tuning.h"
#include "circuitchoosedialog.h"

#include <QGraphicsView>

class PatchSectionView : public QGraphicsView
{
    PatchSection *section;
    QList<CircuitView *>circuitViews;

public:
    PatchSectionView(PatchSection *section);
    ~PatchSectionView() { deletePatchSection(); };
    bool handleKeyPress(int key);
    void mousePressEvent(QMouseEvent *event);
    void addNewCircuit(QString name, jackselection_t jackSelection);
    void addNewJack(QString name);
    QString currentCircuitName() const;

private:
    bool handleMousePress(const QPointF &pos);
    void buildPatchSection();
    void deletePatchSection();
    void rebuildPatchSection();
    CircuitView *currentCircuitView();
    Circuit *currentCircuit();
    const Circuit *currentCircuit() const;
    JackAssignment *currentJackAssignment();
    void moveCursorUpDown(int whence);
    void moveCursorLeftRight(int whence);
    void moveCursorPageUpDown(int whence);
    void deleteCurrentRow();
    void deleteCurrentCircuit();
    void deleteCurrentComment();
    void deleteCurrentJack();
};

#endif // PATCHSECTIONVIEW_H
