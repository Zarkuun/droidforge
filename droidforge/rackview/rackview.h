#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "module.h"
#include "patcheditengine.h"
#include "registermarker.h"
#include "controllerremovaldialog.h"
#include "patchoperator.h"
#include "dragregisterindicator.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>


class RackView : public QGraphicsView, PatchOperator
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

public:
    explicit RackView(PatchEditEngine *patch);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hideRegisterMarker();
    void updateGraphics();

private:
    void addModule(const QString &name, int controllerIndex=-1);
    unsigned numControllers() const;
    void removeModule(int controllerIndex);
    RegisterList findCurrentRegisters() const;
    void updateSize();
    void popupControllerContextMenu(int controller, QString name);
    void popupBackgroundContextMenu();
    void updateRegisterMarker(QPointF, float);
    void removeController(int controllerIndex, QString controllerName,
            RegisterList &atomsToRemap,
            ControllerRemovalDialog::InputHandling ih = ControllerRemovalDialog::INPUT_LEAVE,
            ControllerRemovalDialog::OutputHandling oh = ControllerRemovalDialog::OUTPUT_LEAVE);
    void remapRegisters(
            int controllerIndex,
            RegisterList &atomsToRemap,
            ControllerRemovalDialog::InputHandling inputHandling,
            ControllerRemovalDialog::OutputHandling outputHandling);
    void collectUsedRegisters(int controllerIndex, RegisterList &used);
    bool controllersRegistersUsed(int controllerIndex);
    void updateDragIndicator(QPointF endPos, bool hits, bool suitable);

    bool registersSuitableForSwapping(AtomRegister a, AtomRegister b);
    void swapRegisters(AtomRegister regA, AtomRegister regB);
public slots:
    void modifyPatch();
    void updateRegisterHilites();
    void addController();

private slots:
    void askRemoveController(const QString moduleType, int controller);
    void purchaseController(QString name);
    void moveController(int oldindex, int newindex);
    void remapControls(QString moduleType, int controllerIndex);
    void editLabelling(QString moduleType, int controllerIndex);

signals:
    void registerClicked(AtomRegister ar);
    void patchModified();
};

#endif // RACKVIEW_H
