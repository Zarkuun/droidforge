#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "circuitview.h"
#include "patchsection.h"
#include "tuning.h"
#include "circuitchoosedialog.h"
#include "atomselectordialog.h"

#include <QGraphicsView>
#include <QResizeEvent>

class PatchSectionView : public QGraphicsView
{
    const Patch *patch;
    PatchSection *section;
    float zoomFactor;

    QList<CircuitView *>circuitViews;
    AtomSelectorDialog *atomSelectorDialog; // TODO: Jede Section hat nen eigenen Dialog

public:
    PatchSectionView(const Patch *patch, PatchSection *section, int zoom);
    ~PatchSectionView();
    bool handleKeyPress(int key);
    void addNewCircuit(QString name, jackselection_t jackSelection);
    void addNewJack(QString name);
    QString currentCircuitName() const;
    QString getTitle() const { return section->getNonemptyTitle(); };
    QStringList usedJacks() const;
    void editValue(int key);
    void editCircuitComment(int key);
    bool isEmpty() const;
    void updateCircuits();
    void updateRegisterHilites() const;
    void clickOnRegister(AtomRegister ar);
    void setZoom(int zoom);

protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void updateCursor();
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
    void editCircuit(int key);
    void editJack(int key);
    void editAtom(int key);
    JackAssignment *buildJackAssignment(const QString &jackName);
    QChar keyToChar(int key);
};

#endif // PATCHSECTIONVIEW_H
