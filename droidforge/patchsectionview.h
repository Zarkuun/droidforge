#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "patchsection.h"
#include "tuning.h"
#include "circuitchoosedialog.h"
#include "atomselectordialog.h"

#include <QGraphicsView>

class PatchSectionView : public QGraphicsView
{
    PatchSection *section;
    QList<CircuitView *>circuitViews;
    AtomSelectorDialog *atomSelectorDialog; // TODO: Jede Section hat nen eigenen Dialog

public:
    PatchSectionView(PatchSection *section);
    ~PatchSectionView();;
    bool handleKeyPress(int key);
    void mousePressEvent(QMouseEvent *event);
    void addNewCircuit(QString name, jackselection_t jackSelection);
    void addNewJack(QString name);
    QString currentCircuitName() const;
    QString getTitle() const { return section->getTitle(); };
    QStringList usedJacks() const;
    void editValue(const Patch *patch);
    void editCircuitComment();
    bool isEmpty() const;

private:
    bool handleMousePress(const QPointF &pos);
    void buildPatchSection();
    void deletePatchSection();
    void rebuildPatchSection();
    CircuitView *currentCircuitView();
    const CircuitView *currentCircuitView() const;
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
    void deleteCurrentAtom();
    void editCircuit();
    void editJack();
    void editAtom(const Patch *patch);
    JackAssignment *buildJackAssignment(const QString &jackName);
};

#endif // PATCHSECTIONVIEW_H
