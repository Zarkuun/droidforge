#include "rackview.h"
#include "mainwindow.h"
#include "modulebuilder.h"
#include "tuning.h"
#include "controllerchoosedialog.h"

#include <QGraphicsItem>
#include <QResizeEvent>
#include <QMenu>

RackView::RackView()
    : QGraphicsView()
{
    setFocusPolicy(Qt::NoFocus);
    setMinimumHeight(RACV_MIN_HEIGHT);
    setMaximumHeight(RACV_MAX_HEIGHT * 2);

    QGraphicsScene *thescene = new QGraphicsScene();
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
    QPixmap background(":images/rackbackground.png");
    QBrush brush(background.scaledToHeight(RACV_BACKGROUND_HEIGHT)); //kheight() * 50));
    scene()->setBackgroundBrush(brush);
    setMouseTracking(true);
}

void RackView::resizeEvent(QResizeEvent *)
{
    updateSize();
}

void RackView::setPatch(Patch *newPatch)
{
    patch = newPatch;
    updateGraphics();
}

void RackView::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        QGraphicsItem *item = itemAt(event->pos());
        if (event->button() == Qt::RightButton) {
            if (item->data(DATA_INDEX_CONTROLLER_INDEX).isValid()) {
                popupContextMenu(item->data(DATA_INDEX_CONTROLLER_INDEX).toInt());
            }
            // TODO: Add context menu for adding a controller
        }
        else if (event->button() == Qt::LeftButton) {
            QGraphicsItem *item = itemAt(event->pos());
            if (item == registerMarker)
                the_forge->clickOnRegister(markedRegister);
        }
        else if (!item)
            addController();
    }
}

void RackView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos(); // mapToScene(event->pos()).toPoint();
    QGraphicsItem *item = itemAt(mousePos);
    if (item->data(DATA_INDEX_MODULE_NAME).isValid()) {
        Module *module = (Module *)item;
        QPointF relPos = mapToScene(mousePos) - module->pos();
        AtomRegister *ar = module->registerAt(relPos.toPoint());
        if (ar) {
            QChar t = ar->getRegisterType();
            unsigned n = ar->getNumber() - module->numberOffset(t);
            float diameter = module->registerSize(t, n) * RACV_PIXEL_PER_HP;
            QPointF pos = module->registerPosition(t, n) * RACV_PIXEL_PER_HP;
            updateRegisterMarker(ar, pos + module->pos(), diameter);
            delete ar;
        }
        else
            hideRegisterMarker();
    }
}

void RackView::hiliteRegisters(const RegisterList &registers)
{
    QList<QGraphicsItem *> items = scene()->items();
    for (qsizetype i=0; i<items.count(); i++) {
        QGraphicsItem *gi = items[i];
        if (gi->data(DATA_INDEX_MODULE_NAME).isValid()) {
            Module *module = (Module *)gi;
            unsigned controller = 0;
            if (gi->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
                controller = gi->data(DATA_INDEX_CONTROLLER_INDEX).toInt() + 1;

            module->clearHilites();
            for (qsizetype r=0; r<registers.count(); r++)
            {
                AtomRegister ar = registers[r];
                qDebug() << "REG" << ar.toString();
                if (ar.getController() == controller)
                    module->hiliteRegisters(true, ar.getRegisterType(), ar.getNumber());
                // TODO: Hilite inputs/ouputs
            }
            module->update();
        }
    }
}

void RackView::updateRegisterMarker(AtomRegister *ar, QPointF p, float diameter)
{
    markedRegister = *ar;
    diameter += RACV_REGMARKER_EXTRA_DIAMETER;

    registerMarker->setCenter(p);
    registerMarker->setDiameter(diameter);
    registerMarker->setVisible(true);
    registerMarker->startAnimation();
}

void RackView::hideRegisterMarker()
{
    registerMarker->setVisible(false);
}

void RackView::popupContextMenu(int controller)
{
   QMenu *menu=new QMenu(this);
   menu->addAction(tr("Remove this controller"), this,
                   [this,controller] () {this->removeController(controller); });
   if (controller > 0)
       menu->addAction(tr("Move by position to the left"), this,
                       [this,controller] () {this->moveController(controller, controller-1); });
   if (controller+1 < patch->numControllers())
       menu->addAction(tr("Move by position to the right"), this,
                       [this,controller] () {this->moveController(controller, controller+1); });
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->popup(QCursor::pos());
}

void RackView::removeController(int controller)
{
    // TODO
    qDebug() <<  "WEG MIT" << controller;
}

void RackView::moveController(int fromindex, int toindex)
{
    the_forge->registerEdit(tr("changing order of controllers"));
    patch->swapControllersSmart(fromindex, toindex);
    the_forge->patchHasChanged();
}

void RackView::updateGraphics()
{
    scene()->clear();
    registerMarker = new RegisterMarker();
    scene()->addItem(registerMarker);

    if (!patch)
        return;

    // Add strut, so space above and below the modules is visible
    scene()->addLine(0, 0, 0, RACV_BACKGROUND_HEIGHT, QPen(QColor(0, 0, 0, 0)));

    x = 0;
    addModule("master");
    // if (patch->needG8())
        addModule("g8");
    // if (patch->needX7())
        addModule("x7");
    // addModule("blind");
    x += 200;

    for (qsizetype i=0; i<patch->numControllers(); i++)
        addModule(patch->controller(i), i);
    updateSize();
}

void RackView::updateSize()
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void RackView::addModule(const QString &name, int controllerIndex)
{
    Module *module = ModuleBuilder::buildModule(name);
    module->setData(DATA_INDEX_MODULE_NAME, name);
    scene()->addItem(module);
    if (controllerIndex >= 0)
        module->setData(DATA_INDEX_CONTROLLER_INDEX, controllerIndex);
    module->setPos(x, RACV_TOP_MARGIN);
    x += module->hp() * RACV_PIXEL_PER_HP;
}

void RackView::addController()
{
    QString controller = ControllerChooseDialog::chooseController();
    if (!controller.isEmpty()) {
        QString actionTitle = tr("adding %1 controller").arg(controller.toUpper());
        the_forge->registerEdit(actionTitle);
        patch->addController(controller);
        addModule(controller);
        updateSize();
        the_forge->patchHasChanged();
    }
}
