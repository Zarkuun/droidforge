#include "rackview.h"
#include "colorscheme.h"
#include "controllerlabellingdialog.h"
#include "iconbase.h"
#include "mainwindow.h"
#include "modulebuilder.h"
#include "tuning.h"
#include "controllerchoosedialog.h"
#include "editoractions.h"
#include "updatehub.h"
#include "globals.h"

#include <QGraphicsItem>
#include <QDesktopServices>
#include <QResizeEvent>
#include <QMenu>
#include <QMessageBox>
#include <algorithm>

RackView::RackView(PatchEditEngine *patch)
    : QGraphicsView()
    , PatchView(patch)
    , dragger(this)
{
    setFocusPolicy(Qt::NoFocus);
    setMinimumHeight(RACV_MIN_HEIGHT);
    setMaximumHeight(RACV_MAX_HEIGHT);

    QGraphicsScene *thescene = new QGraphicsScene();
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
    // QPixmap background(":images/background.png");
    // QBrush brush(background.scaledToHeight(1200));
    setMouseTracking(true);
    initScene();

    CONNECT_ACTION(ACTION_NEW_CONTROLLER, &RackView::addController);
    CONNECT_ACTION(ACTION_TOOLBAR_ADD_CONTROLLER, &RackView::addController);
    CONNECT_ACTION(ACTION_ABORT_ALL_ACTIONS, &RackView::abortAllActions);
    CONNECT_ACTION(ACTION_SHOW_REGISTER_LABELS, &RackView::modifyPatch);
    CONNECT_ACTION(ACTION_SHOW_REGISTER_USAGE, &RackView::modifyPatch);

    connectDragger();

    // Events that we create
    connect(this, &RackView::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &RackView::sectionSwitched, the_hub, &UpdateHub::sectionSwitched);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::patchModified, this, &RackView::modifyPatch);
    connect(the_hub, &UpdateHub::sectionSwitched, this, &RackView::updateRegisterHilites);
    connect(the_hub, &UpdateHub::cursorMoved, this, &RackView::updateRegisterHilites);
}

void RackView::modifyPatch()
{
    scene()->setBackgroundBrush(COLOR(COLOR_RACK_BACKGROUND));
    refreshModules();
    updateRegisterHilites();
    dragger.cancel();
}
void RackView::resizeEvent(QResizeEvent *)
{
    updateSize();
}
void RackView::mousePressEvent(QMouseEvent *event)
{
    dragger.mousePress(event);
}
void RackView::mouseReleaseEvent(QMouseEvent *event)
{
    dragger.mouseRelease(event);
}
void RackView::mouseDoubleClickEvent(QMouseEvent *event)
{
    dragger.mousePress(event);
}
void RackView::mouseMoveEvent(QMouseEvent *event)
{
    dragger.mouseMove(event);
}
bool RackView::registersSuitableForSwapping(AtomRegister a, AtomRegister b)
{
    if (a == b)
        return false;
    else if (a.getRegisterType() == b.getRegisterType())
        return true;
    else
        return false;
}
void RackView::swapRegisters(AtomRegister regA, AtomRegister regB)
{
    patch->swapRegisters(regA, regB);
    // Also swap connected registers: Input normalization, LED in Button
    if (regA.getRegisterType() == 'I') {
        AtomRegister nA('N', regA.controller(), regA.number());
        AtomRegister nB('N', regB.controller(), regB.number());
        patch->swapRegisters(nA, nB);
    }
    else if (regA.getRegisterType() == 'B') {
        AtomRegister lA('L', regA.controller(), regA.number());
        AtomRegister lB('L', regB.controller(), regB.number());
        patch->swapRegisters(lA, lB);
    }
    patch->commit(tr("Exchanging registers '%1' and '%2'").arg(regA.toString()).arg(regB.toString()));
    emit patchModified();
}
void RackView::popupControllerContextMenu(int controllerIndex, QString moduleType, AtomRegister areg)
{
   QMenu *menu = new QMenu(this);
   if (controllerIndex >= 0) {
       ADD_ACTION(ACTION_NEW_CONTROLLER, menu);
       menu->addAction(tr("Remove this controller"), this,
                       [this,controllerIndex,moduleType] () {this->askRemoveController(controllerIndex); });
       if (controllerIndex > 0)
           menu->addAction(tr("Move by one position to the left"), this,
                           [this,controllerIndex] () {this->swapControllers(controllerIndex, controllerIndex-1); });
       if (controllerIndex+1 < patch->numControllers())
           menu->addAction(tr("Move by one position to the right"), this,
                           [this,controllerIndex] () {this->swapControllers(controllerIndex, controllerIndex+1); });
       if (controllersRegistersUsed(controllerIndex) && numControllers() >= 2)
           menu->addAction(tr("Move used controls and LEDs to other controllers"),
                           this, [this,controllerIndex,moduleType] () {this->remapControls(moduleType, controllerIndex); });
   }

   menu->addAction(tr("Edit labelling of controls"), this,
                   [this,controllerIndex,moduleType,areg] () {this->editLabelling(moduleType, controllerIndex, areg); });

   if (!areg.isNull()) {
       QAction *action =  menu->addAction(tr("Find this control in your patch"), this,
                       [this,areg] () {this->findRegister(areg); });
       action->setEnabled(patch->controlUsed(areg));
   }

   menu->addSeparator();

   menu->addAction(tr("Lookup this module in the shop"), this,
                   [this,moduleType] () {this->purchaseController(moduleType); });
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->popup(QCursor::pos());
}
void RackView::askRemoveController(int controllerIndex)
{
    // Get a list of all registers that are in use
    RegisterList usedRegisters;
    collectUsedRegisters(controllerIndex, usedRegisters);

    // Nothing used? Simply remove the controller and we are done
    if (usedRegisters.empty()) {
        removeController(controllerIndex);
        return;
    }

    int reply = QMessageBox::No;

    if (patch->numControllers() > 1)
        reply = QMessageBox::question(
                    the_forge,
                    tr("Used controls"),
                    tr("The following controls of this controller are used "
                       "by your patch:\n\n%1\n\nShall I try to move these "
                       "to other controllers?")
                       .arg(usedRegisters.toSmartString()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

    if (reply == QMessageBox::Cancel)
        return;

    // Create a working copy of the patch so we can roll back if
    // the user cancels at a later stage
    Patch *workingPatch = patch->clone();

    if (reply == QMessageBox::Yes)
        workingPatch->moveRegistersToOtherControllers(controllerIndex, usedRegisters);

    if (!usedRegisters.empty()) {
        QString msg;
        if (reply == QMessageBox::Yes)
            msg = tr("I could not move these controls somewhere else:\n\n%1\n\n")
                    .arg(usedRegisters.toSmartString());
        else if (patch->numControllers() == 1)
            msg = tr("The following controls of this controller are used "
                     "by your patch:\n\n%1\n\n")
                    .arg(usedRegisters.toSmartString());
        reply = QMessageBox::question(
                    the_forge,
                    tr("Used controls"),
                    msg + tr("Shall I remove these references from your patch "
                             "in order to avoid chaos and trouble?"),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel) {
            delete workingPatch;
            return;
        }

        if (reply == QMessageBox::Yes)
            workingPatch->removeRegisterReferences(usedRegisters);
    }

    workingPatch->cloneInto(patch);
    delete workingPatch;
    removeController(controllerIndex);
}
void RackView::removeController(int controllerIndex)
{
    QString name = patch->controller(controllerIndex).toUpper();
    patch->removeController(controllerIndex);
    patch->commit(tr("removing %1 controller").arg(name));
    emit patchModified();
}

QPoint RackView::itemPosition(const QGraphicsItem *item, QPoint def)
{
    if (!item)
        return def;
    QRectF rect = item->boundingRect();
    return QPoint(
                (rect.left() + rect.right()) / 2,
                (rect.top() + rect.bottom()) / 2);
}
void RackView::abortAllActions()
{
    dragger.cancel();
}
void RackView::collectUsedRegisters(int controllerIndex, RegisterList &used)
{
    RegisterList allUsedRegisters;
    patch->collectUsedRegisterAtoms(allUsedRegisters); // these are all

    unsigned controller = controllerIndex + 1;
    for (auto& atom: allUsedRegisters) {
        if (atom.getController() == controller && !used.contains(atom))
            used.append(atom);
    }
}
bool RackView::controllersRegistersUsed(int controllerIndex)
{
    RegisterList used;
    collectUsedRegisters(controllerIndex, used);
    return !used.isEmpty();
}
void RackView::updateDragIndicator(QPointF startPos, QPointF endPos, bool hits, bool suitable)
{
    dragRegisterIndicator->abortAnimation();
    dragRegisterIndicator->setPos(startPos);
    dragRegisterIndicator->setEnd(endPos - startPos, hits, suitable);
    dragRegisterIndicator->setVisible(true);
    dragRegisterIndicator->update();
    scene()->update();
}
void RackView::purchaseController(QString name)
{
    QDesktopServices::openUrl(QUrl(SHOP_PRODUCTS_URL + name));
}
void RackView::findRegister(AtomRegister reg)
{
    const Atom *currentAtom = patch->currentAtom();

    bool waitForNext = false;
    bool found = false;

    // First try to find the next atom *after* the current
    Patch::iterator it = patch->begin();
    while (*it) {
        Atom *atom = *it;
        if (atom == currentAtom)
            waitForNext = true;
        else if (waitForNext && atom->isRegister() && reg.belongsTo(*((AtomRegister *)atom))) {
            found = true;
            break;
        }
        ++it;
    }

    // Now try from the start of the patch
    if (!found) {
        it = patch->begin();
        while (*it && *it != currentAtom) {
            Atom *atom = *it;
            if (atom->isRegister() && reg.belongsTo(*((AtomRegister *)atom))) {
                found = true;
                break;
            }
            ++it;
        }
    }

    if (!found)
        return;

    patch->setCursorTo(it.sectionIndex(), it.cursorPosition());
    emit sectionSwitched();
}
void RackView::swapControllers(int fromindex, int toindex)
{
    patch->swapControllersSmart(fromindex, toindex);
    patch->commit(tr("changing controller order"));
    emit patchModified();
}
void RackView::refreshModules()
{
    modules.clear();
    for (auto item: scene()->items())
    {
        if (item->data(DATA_INDEX_MODULE_NAME).isValid() ||
            item->data(DATA_INDEX_REGISTER_NAME).isValid())
            scene()->removeItem(item);
    }
    x = 0;
    addModule("master");
    addModule("g8");
    addModule("x7");
    x += RACV_CONTROLLER_GAP;

    for (qsizetype i=0; i<patch->numControllers(); i++)
        addModule(patch->controller(i), i);

    QPointF margin(RACV_MAIN_MARGIN, RACV_MAIN_MARGIN);
    QRectF bounding(
                modules.first()->moduleRect().topLeft() - margin,
                modules.last()->moduleRect().bottomRight());
    scene()->setSceneRect(bounding);
    ensureVisible(bounding);
}
void RackView::initScene()
{
    registerMarker = new RegisterMarker;
    scene()->addItem(registerMarker);
    dragRegisterIndicator = new DragRegisterIndicator;
    dragRegisterIndicator->setVisible(false);
    scene()->addItem(dragRegisterIndicator);
    dragControllerIndicator = new DragControllerIndicator;
    dragControllerIndicator->setVisible(false);
    scene()->addItem(dragControllerIndicator);
    refreshModules();
    updateSize();
}
void RackView::connectDragger()
{
    connect(&dragger, &MouseDragger::menuOpenedOnBackground, this, &RackView::openMenuOnBackground);
    connect(&dragger, &MouseDragger::menuOpenedOnItem, this, &RackView::openMenuOnItem);
    connect(&dragger, &MouseDragger::hoveredIn, this, &RackView::hoverIn);
    connect(&dragger, &MouseDragger::hoveredOut, this, &RackView::hoverOut);
    connect(&dragger, &MouseDragger::clickedOnItem, this, &RackView::clickOnItem);
    connect(&dragger, &MouseDragger::doubleClickedOnBackground, this, &RackView::doubleClickOnBackground);
    connect(&dragger, &MouseDragger::doubleClickedOnItem, this, &RackView::doubleClickOnItem);
    connect(&dragger, &MouseDragger::itemDragged, this, &RackView::dragItem);
    connect(&dragger, &MouseDragger::itemDraggingStopped, this, &RackView::stopDraggingItem);
    connect(&dragger, &MouseDragger::draggingAborted, this, &RackView::abortDragging);
}
void RackView::updateSize()
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}
void RackView::addModule(const QString &name, int controllerIndex)
{
    Module *module = ModuleBuilder::buildModule(name, patch->getRegisterLabelsPointer());
    module->setData(DATA_INDEX_MODULE_NAME, name);
    module->setData(DATA_INDEX_MODULE_INDEX, modules.count());
    module->setData(DATA_INDEX_DRAGGER_PRIO, 1);
    scene()->addItem(module);
    modules.append(module);
    if (controllerIndex >= 0)
        module->setData(DATA_INDEX_CONTROLLER_INDEX, controllerIndex);
    module->setPos(x, 0); //RACV_TOP_MARGIN);
    module->createRegisterItems(scene(), modules.count() - 1, controllerIndex);
    x += module->hp() * RACV_PIXEL_PER_HP + RACK_MODULE_MARGIN;
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
void RackView::updateRegisterHilites()
{
    RegisterList usedRegisters;
    patch->collectUsedRegisterAtoms(usedRegisters);

    RegisterList currentRegisters;
    const Circuit *circuit = section()->currentCircuit();
    if (circuit)
    {
        CursorPosition cursor = section()->cursorPosition();
        if (cursor.row == ROW_CIRCUIT || cursor.row == ROW_COMMENT) // Circuit selected
            circuit->collectRegisterAtoms(currentRegisters);
        else {
            const JackAssignment *ja = circuit->jackAssignment(cursor.row);
            if (cursor.column == 0)
                ja->collectRegisterAtoms(currentRegisters);
            else {
                const Atom *atom = ja->atomAt(cursor.column);
                if (atom && atom->isRegister())
                    currentRegisters.append(*(AtomRegister *)atom);
            }
        }
    }

    for (auto module: modules) {
        unsigned controller = 0;
        if (module->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
            controller = module->data(DATA_INDEX_CONTROLLER_INDEX).toInt() + 1;

        module->clearHilites();
        for (auto ar: usedRegisters)
        {
            if (ar.getController() == controller)
                module->hiliteRegisters(1, ar.getRegisterType(), ar.getNumber());
        }
        for (auto ar: currentRegisters)
        {
            if (ar.getController() == controller)
                module->hiliteRegisters(2, ar.getRegisterType(), ar.getNumber());
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

void RackView::clickOnItem(QGraphicsItem *item)
{
    if (item->data(DATA_INDEX_REGISTER_NAME).isValid()) {
        AtomRegister ar(item->data(DATA_INDEX_REGISTER_NAME).toString());
        emit registerClicked(ar);
    }
}

void RackView::doubleClickOnItem(QGraphicsItem *item)
{
    if (item->data(DATA_INDEX_MODULE_INDEX).isValid()) {
        Module *module = modules[item->data(DATA_INDEX_MODULE_INDEX).toInt()];
        int controllerIndex = item->data(DATA_INDEX_CONTROLLER_INDEX).toInt();
        AtomRegister ar;
        if (item->data(DATA_INDEX_REGISTER_NAME).isValid())
            ar = AtomRegister(item->data(DATA_INDEX_REGISTER_NAME).toString());
        editLabelling(module->getName(), controllerIndex, ar);
    }
}

void RackView::doubleClickOnBackground()
{
    TRIGGER_ACTION(ACTION_NEW_CONTROLLER);
}

void RackView::openMenuOnBackground()
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    ADD_ACTION(ACTION_NEW_CONTROLLER, menu);
    menu->popup(QCursor::pos());
}

void RackView::openMenuOnItem(QGraphicsItem *item)
{
    QVariant v = item->data(DATA_INDEX_CONTROLLER_INDEX);
    int index = v.isValid() ? v.toInt() : -1;
    AtomRegister areg;
    if (item->data(DATA_INDEX_REGISTER_NAME).isValid())
        areg = item->data(DATA_INDEX_REGISTER_NAME).toString();

                        popupControllerContextMenu(index, item->data(DATA_INDEX_MODULE_NAME).toString(), areg);
}

void RackView::hoverIn(QGraphicsItem *item)
{
    if (!item->data(DATA_INDEX_REGISTER_NAME).isValid())
        return;

    auto rect = item->boundingRect();
    float diameter = rect.width() + RACV_REGMARKER_EXTRA_DIAMETER;
    int x = (rect.left() + rect.right()) / 2;
    int y = (rect.top() + rect.bottom()) / 2;
    registerMarker->setPos(x, y);
    registerMarker->setDiameter(diameter);
    registerMarker->setVisible(true);
    registerMarker->update();
    registerMarker->startAnimation();
}

void RackView::hoverOut(QGraphicsItem *item)
{
    if (!item->data(DATA_INDEX_REGISTER_NAME).isValid())
        return;
    registerMarker->setVisible(false);
}

void RackView::dragItem(QGraphicsItem *startItem, QGraphicsItem *item, QPoint endPos)
{
    if (startItem->data(DATA_INDEX_REGISTER_NAME).isValid())
        dragRegister(startItem, item, endPos);
    else if (startItem->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
        dragController(startItem, item, endPos);
}

void RackView::dragRegister(QGraphicsItem *startItem, QGraphicsItem *item, QPoint endPos)
{
    setCursor(Qt::ClosedHandCursor);
    AtomRegister fromReg(startItem->data(DATA_INDEX_REGISTER_NAME).toString());
    QPoint startPos = itemPosition(startItem);
    bool hits = false;
    bool suitable = false;
    if (item && item->data(DATA_INDEX_REGISTER_NAME).isValid()) {
        endPos = itemPosition(item);
        hits = true;
        AtomRegister toReg(item->data(DATA_INDEX_REGISTER_NAME).toString());
        suitable = registersSuitableForSwapping(fromReg, toReg);
    }
    dragRegisterIndicator->abortAnimation();
    dragRegisterIndicator->setPos(startPos);
    dragRegisterIndicator->setEnd(endPos - startPos, hits, suitable);
    dragRegisterIndicator->setVisible(true);
    dragRegisterIndicator->update();
    scene()->update();
}

void RackView::dragController(QGraphicsItem *startItem, QGraphicsItem *, QPoint endPos)
{
    setCursor(Qt::ClosedHandCursor);
    int moduleIndex = startItem->data(DATA_INDEX_MODULE_INDEX).toInt();
    int controllerIndex = startItem->data(DATA_INDEX_CONTROLLER_INDEX).toInt();
    float indicatorPos = endPos.x();
    int ip = snapControllerInsertPosition(controllerIndex, endPos.x(), &indicatorPos);

    Module *module = modules[moduleIndex];
    dragControllerIndicator->abortAnimation();
    dragControllerIndicator->setControllerRect(module->moduleRect().translated(module->pos()));
    dragControllerIndicator->setInsertPos(indicatorPos, ip >= 0);
    dragControllerIndicator->setVisible(true);
    dragControllerIndicator->update();
    scene()->update();
}

void RackView::stopDraggingItem(QGraphicsItem *startItem, QGraphicsItem *item, QPoint pos)
{
    unsetCursor();
    if (startItem->data(DATA_INDEX_REGISTER_NAME).isValid()
        &&   item->data(DATA_INDEX_REGISTER_NAME).isValid())
        stopDraggingRegister(startItem, item);

    else if (startItem->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
        stopDraggingController(startItem, pos);

    dragRegisterIndicator->setVisible(false);
    dragControllerIndicator->setVisible(false);
}
void RackView::stopDraggingRegister(QGraphicsItem *startItem, QGraphicsItem *item)
{
    AtomRegister fromReg(startItem->data(DATA_INDEX_REGISTER_NAME).toString());
    AtomRegister toReg(item->data(DATA_INDEX_REGISTER_NAME).toString());
    if (registersSuitableForSwapping(fromReg, toReg)) {
        swapRegisters(fromReg, toReg);
        dragRegisterIndicator->doSuccessAnimation();
        return;
    }
}
void RackView::stopDraggingController(QGraphicsItem *startItem, QPoint pos)
{
    int controllerIndex = startItem->data(DATA_INDEX_CONTROLLER_INDEX).toInt();
    float indicatorPos = pos.x();
    int ip = snapControllerInsertPosition(controllerIndex, pos.x(), &indicatorPos);
    if (ip >= 0) {
        if (ip > controllerIndex)
            ip --;
        patch->moveControllerSmart(controllerIndex, ip);
        patch->commit(tr("moving controller"));
        emit patchModified();
    }
    dragControllerIndicator->setVisible(false);
}

int RackView::snapControllerInsertPosition(int fromIndex, float x, float *insertSnap) const
{

    for (auto module: modules) {
        if (module->data(DATA_INDEX_CONTROLLER_INDEX).isValid()) {
            int i = module->data(DATA_INDEX_CONTROLLER_INDEX).toInt();
            if (i == fromIndex)
                continue;
            int snap_distance = RACV_CONTROLLER_SNAP_DISTANCE;
            // Make it easier to drag it to the utter right
            if (i == patch->numControllers() - 1 && x > module->boundingRect().right()) {
                snap_distance *= 3;
            }

            float left = module->pos().x();
            if (fromIndex != i-1 && qAbs(left - x) < snap_distance) {
                *insertSnap = left;
                return i;
            }
            float right = left + module->moduleRect().width();
            if (fromIndex != i+1 && qAbs(right - x) < snap_distance) {
                *insertSnap = right;
                return i + 1;
            }
        }
    }
    return -1;
}
void RackView::abortDragging()
{
    unsetCursor();
    dragRegisterIndicator->setVisible(false);
    dragControllerIndicator->setVisible(false);
}
void RackView::remapControls(QString controllerName, int controllerIndex)
{
    RegisterList atomsToRemap;
    collectUsedRegisters(controllerIndex, atomsToRemap);
    unsigned num = atomsToRemap.count();
    patch->moveRegistersToOtherControllers(controllerIndex, atomsToRemap);
    if (!atomsToRemap.isEmpty())
    {
        QString listing;
        for (auto& entry: atomsToRemap)
            listing += " " + entry.toString();

        QMessageBox box(
                    QMessageBox::Warning,
                    tr("Not all controlles remapped"),
                    tr("The following registers have not been remapped, since there "
                       "was nothing free: %1").arg(listing),
                    QMessageBox::Ok,
                    this);
        box.exec();
    }
    // Only create a commit if something has been moved
    if (num != atomsToRemap.count()) {
        patch->commit(tr("moving used controls of %1").arg(controllerName.toUpper()));
        emit patchModified();
    }
}
void RackView::editLabelling(QString moduleType, int controllerIndex, AtomRegister reg)
{
    // Get current position of register marker
    RegisterLabels &labels = patch->getRegisterLabels();
    ControllerLabellingDialog dialog(labels, moduleType, controllerIndex + 1, reg, this);
    int ret = dialog.exec();
    if (ret == QDialog::Accepted) {
        patch->commit(tr("editing labelling of '%1'").arg(moduleType));
        emit patchModified();
    }
}
