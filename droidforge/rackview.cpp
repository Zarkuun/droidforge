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
                qDebug() <<  item->data(0);
                popupContextMenu(item->data(0).toInt());
            }
            // TODO: Add context menu for adding a controller
        }
        else if (!item)
            addController();
    }
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
    qDebug() <<  "WEG MIT" << controller;
}

void RackView::moveController(int fromindex, int toindex)
{
    the_forge->registerEdit(tr("changing order of controllers"));
    patch->reorderControllersSmart(fromindex, toindex);
    the_forge->patchHasChanged();
    updateGraphics();
}

void RackView::updateGraphics()
{
    scene()->clear();
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
    QPixmap *image = module->faceplateImage();
    QGraphicsItem *gi = scene()->addPixmap(*image);
    if (controllerIndex >= 0)
        gi->setData(0, controllerIndex);
    gi->setPos(x, RACV_TOP_MARGIN);
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
