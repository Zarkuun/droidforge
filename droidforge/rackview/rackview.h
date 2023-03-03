#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "module.h"
#include "mousedragger.h"
#include "patcheditengine.h"
#include "registermarker.h"
#include "patchview.h"
#include "dragregisterindicator.h"
#include "dragcontrollerindicator.h"
#include "controllerchoosedialog.h"
#include "modulebuilder.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QTimer>


class MainWindow;

class RackView : public QGraphicsView, PatchView
{
    Q_OBJECT

    MainWindow *mainWindow;
    ModuleBuilder moduleBuilder;
    ControllerChooseDialog controllerChooseDialog;

    QList<Module *> modules;
    int x;
    RegisterMarker *registerMarker;
    DragRegisterIndicator *dragRegisterIndicator;
    DragControllerIndicator *dragControllerIndicator;
    QTimer *registerHilightTimer;
    MouseDragger dragger;
    int previousHeight;

public:
    explicit RackView(MainWindow *mainWindow, PatchEditEngine *patch);
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void initScene();

private:
    void connectDragger();
    void addModule(const QString &name, int controllerIndex=-1);
    unsigned numControllers() const;
    void removeModule(int controllerIndex);
    RegisterList findCurrentRegisters() const;
    void updateSize();
    void popupModuleContextMenu(int controller, QString name, AtomRegister areg);
    void popupBackgroundContextMenu();
    void collectUsedRegisters(int controllerIndex, RegisterList &used);
    bool controllersRegistersUsed(int controllerIndex);
    void updateDragIndicator(QPointF startPos, QPointF endPos, bool hits, bool suitable);
    bool registersSuitableForSwapping(AtomRegister a, AtomRegister b);
    void swapRegisters(AtomRegister regA, AtomRegister regB);
    void refreshScene();
    void removeController(int controllerIndex);
    QPoint itemPosition(const QGraphicsItem *item, QPoint def=QPoint(0,0));
    void dragRegister(QGraphicsItem *startItem, QGraphicsItem *item, QPoint endPos);
    void dragController(QGraphicsItem *startItem, QGraphicsItem *item, QPoint endPos);
    void stopDraggingRegister(QGraphicsItem *startItem, QGraphicsItem *item);
    void stopDraggingController(QGraphicsItem *startItem, QPoint pos);
    int snapControllerInsertPosition(int fromIndex, float x, float *insertSnap) const;
    void updateModuleHeights();

public slots:
    void abortAllActions();
    void modifyPatch();
    void toggleDisplayOptions();
    void updateRegisterHilites();
    void addController();

    // from mouseDragger
    void clickOnItem(QGraphicsItem *item);
    void doubleClickOnItem(QGraphicsItem *item);
    void doubleClickOnBackground();
    void openMenuOnBackground();
    void openMenuOnItem(QGraphicsItem *item);
    void hoverIn(QGraphicsItem *item);
    void hoverOut(QGraphicsItem *item);
    void dragItem(QGraphicsItem *startItem, QGraphicsItem *item, QPoint pos);
    void stopDraggingItem(QGraphicsItem *startItem, QGraphicsItem *item, QPoint pos);
    void abortDragging();

private slots:
    void setRegisterHilitesDirty();
    void duplicateController(int controller, bool withLabels);
    void askRemoveController(int controller);
    void purchaseController(QString name);
    void findRegister(AtomRegister reg);
    void swapControllers(int oldindex, int newindex);
    void remapControls(QString moduleType, int controllerIndex);
    void editLabelling(QString moduleType, int controllerIndex, AtomRegister reg);

signals:
    void registerClicked(AtomRegister ar);
    void patchModified();
    void sectionSwitched();
};

#endif // RACKVIEW_H
