#include "rackview.h"
#include "mainwindow.h"
#include "modulebuilder.h"
#include "tuning.h"
#include "controllerchoosedialog.h"
#include "controllerremovaldialog.h"

#include <QGraphicsItem>
#include <QDesktopServices>
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
            if (item->data(DATA_INDEX_MODULE_NAME).isValid()) {
                int index = -1;
                if (item->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
                    index = item->data(DATA_INDEX_CONTROLLER_INDEX).toInt();
                popupContextMenu(index, item->data(DATA_INDEX_MODULE_NAME).toString());
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
    for (auto module: modules) {
        unsigned controller = 0;
        if (module->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
            controller = module->data(DATA_INDEX_CONTROLLER_INDEX).toInt() + 1;

        module->clearHilites();
        for (qsizetype r=0; r<registers.count(); r++)
        {
            AtomRegister ar = registers[r];
            if (ar.getController() == controller)
                module->hiliteRegisters(true, ar.getRegisterType(), ar.getNumber());
            // TODO: Hilite inputs/ouputs
        }
        module->update();
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

void RackView::removeController(
        int controllerIndex,
        QString controllerName,
        RegisterList &atomsToRemap,
        ControllerRemovalDialog::InputHandling inputHandling,
        ControllerRemovalDialog::OutputHandling  outputHandling)
{
    the_forge->registerEdit(tr("removing %1 controller").arg(controllerName.toUpper()));
    qDebug() << "REMOVE" << controllerIndex << atomsToRemap << inputHandling << outputHandling;
    remapRegisters(controllerIndex, atomsToRemap, inputHandling, outputHandling);
    qDebug() << "NOT REMAPPED:" << atomsToRemap;
    the_forge->patchHasChanged();
}

void RackView::remapRegisters(
        int controllerIndex,
        RegisterList &atomsToRemap,
        ControllerRemovalDialog::InputHandling , // inputHandling,
        ControllerRemovalDialog::OutputHandling ) // outputHandling)
{
    unsigned controller = controllerIndex + 1;

    // Get list of all registers.
    RegisterList allRegisters;
    collectAllRegisters(allRegisters); // all including our controller

    RegisterList remapFrom;
    RegisterList remapTo;

    // Loop through all registers to be remapped
    for (auto& toRemap: atomsToRemap)
    {
        qDebug() << "remapping" << toRemap;
        // Loop through all candidate registers
        for (auto &candidate: allRegisters) {
            if (candidate.getController() == controller)
                continue; // Don't remap to ourselves
            if (toRemap.getRegisterType() == candidate.getRegisterType())
            // TODO: remapp G to I or O, but then we need to known
            // wether G is used as an input or output.
            {
                remapFrom.append(toRemap);
                remapTo.append(candidate);
                allRegisters.removeAll(candidate);
                atomsToRemap.removeAll(toRemap);
                break;
            }
        }
    }

    // Apply this remapping
    for (unsigned i=0; i<remapFrom.size(); i++) {
        qDebug() << remapFrom[i].toString() << " -> " << remapTo[i];
        patch->remapRegister(remapFrom[i], remapTo[i]);
    }
}

void RackView::collectAllRegisters(RegisterList &rl) const
{
    for (auto module: modules) {
        module->collectAllRegisters(rl);
    }
}

void RackView::hideRegisterMarker()
{
    registerMarker->setVisible(false);
}

void RackView::popupContextMenu(int controllerIndex, QString name)
{
   QMenu *menu=new QMenu(this);
   if (controllerIndex >= 0) {
       menu->addAction(the_forge->icon("delete"), tr("Remove this controller"), this,
                       [this,controllerIndex,name] () {this->askRemoveController(controllerIndex, name); });
       if (controllerIndex > 0)
           menu->addAction(the_forge->icon("keyboard_arrow_left"), tr("Move by one position to the left"), this,
                           [this,controllerIndex] () {this->moveController(controllerIndex, controllerIndex-1); });
       if (controllerIndex+1 < patch->numControllers())
           menu->addAction(the_forge->icon("keyboard_arrow_right"), tr("Move by one position to the right"), this,
                           [this,controllerIndex] () {this->moveController(controllerIndex, controllerIndex+1); });
       menu->addSeparator();
   }
   menu->addAction(the_forge->icon("purchase"), tr("Lookup this module in the shop"), this,
                   [this,name] () {this->purchaseController(name); });
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->popup(QCursor::pos());
}

void RackView::askRemoveController(int controllerIndex, const QString name)
{
    // Get a list of all registers that are in use
    unsigned controller = controllerIndex + 1;
    RegisterList atomsToRemap;
    RegisterList rl;
    patch->collectRegisterAtoms(rl);
    for (auto& atom: rl) {
        if (atom.getController() == controller)
            atomsToRemap.append(atom);
    }
    if (atomsToRemap.empty())
        removeController(controllerIndex, name, atomsToRemap);

    else {
        static ControllerRemovalDialog *dialog = 0;
        if (!dialog)
            dialog = new ControllerRemovalDialog(this);

        dialog->setRegistersToRemap(atomsToRemap);
        if (dialog->exec() == QDialog::Accepted) {
            bool remap = dialog->shouldRemap();
            if (!remap)
                atomsToRemap.clear();
            auto ih = dialog->inputHandling();
            auto oh = dialog->outputHandling();
            removeController(controllerIndex, name, atomsToRemap, ih, oh);
        }
    }
}

void RackView::purchaseController(QString name)
{
    QDesktopServices::openUrl(QUrl(SHOP_PRODUCTS_URL + name));
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
    modules.clear();
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
    modules.append(module);
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
