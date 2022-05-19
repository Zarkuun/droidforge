#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "module.h"
#include "patch.h"
#include "registermarker.h"
#include "controllerremovaldialog.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>


class RackView : public QGraphicsView
{
    Q_OBJECT

    Patch *patch;
    QList<Module *> modules;
    unsigned x;
    RegisterMarker *registerMarker;
    AtomRegister markedRegister;

public:
    explicit RackView();
    void resizeEvent(QResizeEvent *event);
    void setPatch(Patch *patch);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hiliteRegisters(const RegisterList &registers);
    void hideRegisterMarker();

private:
    void addModule(const QString &name, int controllerIndex=-1);
    void updateGraphics();
    void updateSize();
    void popupContextMenu(int controller, QString name);
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
    void collectAllRegisters(RegisterList &rl) const;

public slots:
    void addController();

private slots:
    void askRemoveController(int controller, const QString name);
    void purchaseController(QString name);
    void moveController(int oldindex, int newindex);
};

#endif // RACKVIEW_H
