#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "module.h"
#include "patcheditengine.h"
#include "registermarker.h"
#include "controllerremovaldialog.h"
#include "patchoperator.h"

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

public:
    explicit RackView(PatchEditEngine *patch);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
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
    void updateRegisterMarker(AtomRegister *markedAtom, QPointF, float);
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

public slots:
    void modifyPatch();
    void updateRegisterMarkers();
    void addController();

private slots:
    void askRemoveController(int controller, const QString name);
    void purchaseController(QString name);
    void moveController(int oldindex, int newindex);
    void remapControls(int controllerIndex, QString controllerName);
    void editLabelling(int controllerIndex);

signals:
    void registerClicked(AtomRegister ar);
    void patchModified();
};

#endif // RACKVIEW_H
