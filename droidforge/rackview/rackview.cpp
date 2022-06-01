#include "rackview.h"
#include "mainwindow.h"
#include "modulebuilder.h"
#include "tuning.h"
#include "controllerchoosedialog.h"
#include "controllerremovaldialog.h"
#include "editoractions.h"
#include "updatehub.h"

#include <QGraphicsItem>
#include <QDesktopServices>
#include <QResizeEvent>
#include <QMenu>
#include <QMessageBox>
#include <algorithm>

RackView::RackView(PatchEditEngine *patch)
    : QGraphicsView()
    , PatchOperator(patch)
{
    setFocusPolicy(Qt::NoFocus);
    setMinimumHeight(RACV_MIN_HEIGHT);
    setMaximumHeight(RACV_MAX_HEIGHT);

    QGraphicsScene *thescene = new QGraphicsScene();
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
    QPixmap background(":images/rackbackground.png");
    QBrush brush(background.scaledToHeight(RACV_BACKGROUND_HEIGHT)); //kheight() * 50));
    scene()->setBackgroundBrush(brush);
    setMouseTracking(true);

    CONNECT_ACTION(ACTION_ADD_CONTROLLER, &RackView::addController);

    // Events that we create
    connect(this, &RackView::patchModified, the_hub, &UpdateHub::modifyPatch);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::patchModified, this, &RackView::modifyPatch);
    connect(the_hub, &UpdateHub::sectionSwitched, this, &RackView::updateRegisterMarkers);
    connect(the_hub, &UpdateHub::cursorMoved, this, &RackView::updateRegisterMarkers);
}

void RackView::modifyPatch()
{
    updateGraphics();
    updateRegisterMarkers();
}

void RackView::resizeEvent(QResizeEvent *)
{
    updateSize();
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
                popupControllerContextMenu(index, item->data(DATA_INDEX_MODULE_NAME).toString());
            }
            else if (!item)
                popupBackgroundContextMenu();
        }
        else if (event->button() == Qt::LeftButton) {
            QGraphicsItem *item = itemAt(event->pos());
            if (item == registerMarker)
                emit registerClicked(markedRegister);
        }
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
            unsigned n = ar->number() - module->numberOffset(t);
            float diameter = module->registerSize(t, n) * RACV_PIXEL_PER_HP;
            QPointF pos = module->registerPosition(t, n) * RACV_PIXEL_PER_HP;
            updateRegisterMarker(ar, pos + module->pos(), diameter);
            delete ar;
        }
        else
            hideRegisterMarker();
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

void RackView::remapRegisters(
        int controllerIndex,
        RegisterList &atomsToRemap,
        ControllerRemovalDialog::InputHandling , // inputHandling,
        ControllerRemovalDialog::OutputHandling ) // outputHandling)
{
    unsigned controller = controllerIndex + 1;

    // Get list of all registers.
    RegisterList allRegisters;
    patch->collectAvailableRegisterAtoms(allRegisters);

    RegisterList remapFrom;
    RegisterList remapTo;
    RegisterList remapped;

    // Loop through all registers to be remapped
    for (auto& toRemap: atomsToRemap)
    {
        // Loop through all candidate registers
        for (auto &candidate: allRegisters) {
            if (candidate.controller() == controller)
                continue; // Don't remap to ourselves
            // TODO: Fehlt da nicht ein check, ob das schon belegt ist?
            if (toRemap.getRegisterType() == candidate.getRegisterType())
            // TODO: remapp G to I or O, but then we need to known
            // wether G is used as an input or output.
            {
                remapFrom.append(toRemap);
                remapTo.append(candidate);
                allRegisters.removeAll(candidate);
                remapped.append(toRemap);
                break;
            }
        }
    }

    for (auto& atom: remapped)
        atomsToRemap.removeAll(atom);

    // Apply this remapping
    for (unsigned i=0; i<remapFrom.size(); i++)
        patch->remapRegister(remapFrom[i], remapTo[i]);
}

void RackView::hideRegisterMarker()
{
    registerMarker->setVisible(false);
}

void RackView::popupControllerContextMenu(int controllerIndex, QString name)
{
   QMenu *menu = new QMenu(this);
   if (controllerIndex >= 0) {
       ADD_ACTION(ACTION_ADD_CONTROLLER, menu);
       menu->addAction(the_forge->icon("delete"), tr("Remove this controller"), this,
                       [this,controllerIndex,name] () {this->askRemoveController(controllerIndex, name); });
       if (controllerIndex > 0)
           menu->addAction(the_forge->icon("keyboard_arrow_left"), tr("Move by one position to the left"), this,
                           [this,controllerIndex] () {this->moveController(controllerIndex, controllerIndex-1); });
       if (controllerIndex+1 < patch->numControllers())
           menu->addAction(the_forge->icon("keyboard_arrow_right"), tr("Move by one position to the right"), this,
                           [this,controllerIndex] () {this->moveController(controllerIndex, controllerIndex+1); });
       if (controllersRegistersUsed(controllerIndex) && numControllers() >= 2)
           menu->addAction(tr("Move used controls and LEDs to other controllers"),
                           this, [this,controllerIndex,name] () {this->remapControls(controllerIndex, name); });
   }
   if (!menu->isEmpty())
       menu->addSeparator();
   menu->addAction(the_forge->icon("purchase"), tr("Lookup this module in the shop"), this,
                   [this,name] () {this->purchaseController(name); });
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->popup(QCursor::pos());
}

void RackView::popupBackgroundContextMenu()
{
   QMenu *menu = new QMenu(this);
   ADD_ACTION(ACTION_ADD_CONTROLLER, menu);
   menu->popup(QCursor::pos());
}

void RackView::askRemoveController(int controllerIndex, const QString name)
{
    // Get a list of all registers that are in use
    RegisterList atomsToRemap;
    collectUsedRegisters(controllerIndex, atomsToRemap);
    if (atomsToRemap.empty())
        removeController(controllerIndex, name, atomsToRemap);

    else {
        std::sort(atomsToRemap.begin(), atomsToRemap.end());
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

void RackView::collectUsedRegisters(int controllerIndex, RegisterList &used)
{
    RegisterList allUsedRegisters;
    patch->collectUsedRegisterAtoms(allUsedRegisters); // these are all

    unsigned controller = controllerIndex + 1;
    for (auto& atom: allUsedRegisters) {
        if (atom.controller() == controller && !used.contains(atom))
            used.append(atom);
    }
}

bool RackView::controllersRegistersUsed(int controllerIndex)
{
    RegisterList used;
    collectUsedRegisters(controllerIndex, used);
    return !used.isEmpty();
}

void RackView::purchaseController(QString name)
{
    QDesktopServices::openUrl(QUrl(SHOP_PRODUCTS_URL + name));
}

void RackView::moveController(int fromindex, int toindex)
{
    patch->swapControllersSmart(fromindex, toindex);
    patch->commit(tr("changing controller order"));
    emit patchModified();
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

unsigned RackView::numControllers() const
{
    unsigned n=0;
    for (auto module: modules)
        if (module->isController())
            n++;
    return n;
}

void RackView::removeModule(int controllerIndex)
{
    Module *module = modules[controllerIndex];
    scene()->removeItem(module);
    modules.remove(controllerIndex);
}

void RackView::updateRegisterMarkers()
{
    const Circuit *circuit = section()->currentCircuit();
    if (!circuit)
        return;

    RegisterList registers;
    CursorPosition cursor = section()->cursorPosition();
    if (cursor.row == -2 || cursor.row == -1) // Circuit selected
        circuit->collectRegisterAtoms(registers);
    else {
        const JackAssignment *ja = circuit->jackAssignment(cursor.row);
        if (cursor.column == 0)
            ja->collectRegisterAtoms(registers);
        else {
            const Atom *atom = ja->atomAt(cursor.column);
            if (atom && atom->isRegister())
                registers.append(*(AtomRegister *)atom);
        }
    }

    for (auto module: modules) {
        unsigned controller = 0;
        if (module->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
            controller = module->data(DATA_INDEX_CONTROLLER_INDEX).toInt() + 1;

        module->clearHilites();
        for (qsizetype r=0; r<registers.count(); r++)
        {
            AtomRegister ar = registers[r];
            if (ar.controller() == controller)
                module->hiliteRegisters(true, ar.getRegisterType(), ar.number());
            // TODO: Hilite inputs/ouputs
        }
        module->update();
    }
}

void RackView::addController()
{
    QString controller = ControllerChooseDialog::chooseController();
    if (!controller.isEmpty()) {
        patch->addController(controller);
        patch->commit(tr("adding %1 controller").arg(controller.toUpper()));
        emit patchModified();
    }
}

void RackView::removeController(
        int controllerIndex,
        QString controllerName,
        RegisterList &atomsToRemap,
        ControllerRemovalDialog::InputHandling inputHandling,
        ControllerRemovalDialog::OutputHandling  outputHandling)
{
    remapRegisters(controllerIndex, atomsToRemap, inputHandling, outputHandling);

    if (!atomsToRemap.isEmpty()) {
        patch->removeRegisterReferences(
                    atomsToRemap,
                    inputHandling,
                    outputHandling);
    }

    patch->removeController(controllerIndex);
    patch->commit(tr("removing %1 controller").arg(controllerName.toUpper()));
    emit patchModified();
}


void RackView::remapControls(int controllerIndex, QString controllerName)
{
    RegisterList atomsToRemap;
    collectUsedRegisters(controllerIndex, atomsToRemap);
    remapRegisters(controllerIndex, atomsToRemap, ControllerRemovalDialog::INPUT_LEAVE, ControllerRemovalDialog::OUTPUT_LEAVE);
    if (!atomsToRemap.isEmpty())
    {
        QString listing;
        for (auto& entry: atomsToRemap)
            listing += " " + entry.toString();

        QMessageBox box(
                    QMessageBox::Warning,
                    tr("Not all controlles remapped"),
                    tr("The following registers havea not been remapped, since there "
                       "was nothing free: %1").arg(listing),
                    QMessageBox::Ok,
                    this);
        box.exec();
    }
    patch->commit(tr("moving used controls of %1").arg(controllerName.toUpper()));
    emit patchModified();
}
