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
            if (item->data(0).isValid()) {
                popupContextMenu(item->data(0).toInt());
            }
            // TODO: Add context menu for adding a controller
        }
        else if (!item)
            addController();
    }
}

void RackView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos(); // mapToScene(event->pos()).toPoint();
    QGraphicsItem *item = itemAt(mousePos);
    if (item->data(1).isValid()) {
        Module *module = (Module *)item;
        QPointF relPos = mapToScene(mousePos) - module->pos();
        AtomRegister *ar = module->registerAt(relPos.toPoint());
        if (ar) {
            QChar t = ar->getRegisterType();
            unsigned n = ar->getNumber();
            float diameter = module->controlSize(t, n) * RACV_PIXEL_PER_HP;
            QPointF pos = module->controlPosition(t, n) * RACV_PIXEL_PER_HP;
            updateRegisterMarker(pos + module->pos(), diameter);
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
        if (gi->data(1).isValid()) {
            Module *module = (Module *)gi;
            unsigned controller = 0;
            if (gi->data(0).isValid())
                controller = gi->data(0).toInt() + 1;

            // QString name = gi->data(1).toString();
            module->clearHilites();
            for (qsizetype r=0; r<registers.count(); r++)
            {
                AtomRegister ar = registers[r];
                if (ar.getController() == controller)
                    module->hiliteControls(true, ar.getRegisterType(), ar.getNumber());
                // TODO: Hilite inputs/ouputs
            }
            module->update();
        }
    }
}

void RackView::updateRegisterMarker(QPointF p, float diameter)
{
    diameter += RACV_REGMARKER_EXTRA_DIAMETER;
    QPointF pos(p.x() - diameter/2, p.y() - diameter/2);
    QRectF r(pos, QSizeF(diameter, diameter));
    registerMarker->setRect(r);
    registerMarker->setVisible(true);
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
    QPen pen;
    pen.setWidth(RACV_REGMARKER_PEN_WIDTH);
    pen.setStyle(RACV_REGMARKER_PEN_STYLE);
    pen.setColor(RACV_REGMARKER_PEN_COLOR);
    registerMarker = scene()->addEllipse(QRect(0, 0, 0, 0), pen);
    registerMarker->setPen(pen);
    registerMarker->setBrush(RACV_REGMARKER_BACKGROUND);
    registerMarker->setZValue(50);

    if (!patch)
        return;

    // Add strut, so space above and below the modules is visible
    scene()->addLine(0, 0, 0, RACV_BACKGROUND_HEIGHT, QPen(QColor(0, 0, 0, 0)));

    x = 10;
    addModule("master");
    if (patch->needG8())
        addModule("g8");
    if (patch->needX7())
        addModule("x7");
    addModule("blind");

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
    module->setData(1, name);
    scene()->addItem(module);
    if (controllerIndex >= 0)
        module->setData(0, controllerIndex);
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
