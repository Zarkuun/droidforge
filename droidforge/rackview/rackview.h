#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "module.h"
#include "patcheditengine.h"
#include "registermarker.h"
#include "patchview.h"
#include "dragregisterindicator.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>


class RackView : public QGraphicsView, PatchView
{
    Q_OBJECT

    QList<Module *> modules;
    unsigned x;
    RegisterMarker *registerMarker;
    AtomRegister markedRegister;
    DragRegisterIndicator *dragRegisterIndicator;

    bool dragging;
    bool draggedAtRegister;
    AtomRegister draggingStartRegister;
    QPointF draggingStartPosition;
    float maxDistanceFromMouseDown;

public:
    explicit RackView(PatchEditEngine *patch);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hideRegisterMarker();
    void initScene();

private:
    void addModule(const QString &name, int controllerIndex=-1);
    unsigned numControllers() const;
    void removeModule(int controllerIndex);
    RegisterList findCurrentRegisters() const;
    void updateSize();
    void popupControllerContextMenu(int controller, QString name);
    void popupBackgroundContextMenu();
    void updateRegisterMarker(QPointF, float);
    void collectUsedRegisters(int controllerIndex, RegisterList &used);
    bool controllersRegistersUsed(int controllerIndex);
    void updateDragIndicator(QPointF endPos, bool hits, bool suitable);
    bool registersSuitableForSwapping(AtomRegister a, AtomRegister b);
    void swapRegisters(AtomRegister regA, AtomRegister regB);
    void refreshModules();
    void removeController(int controllerIndex);

public slots:
    void modifyPatch();
    void updateRegisterHilites();
    void addController();

private slots:
    void askRemoveController(int controller);
    void purchaseController(QString name);
    void findRegister(AtomRegister reg);
    void moveController(int oldindex, int newindex);
    void remapControls(QString moduleType, int controllerIndex);
    void editLabelling(QString moduleType, int controllerIndex, AtomRegister reg);

signals:
    void registerClicked(AtomRegister ar);
    void patchModified();
    void sectionSwitched();
};

#endif // RACKVIEW_H
