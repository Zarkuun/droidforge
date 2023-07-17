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

RackView::RackView(MainWindow *mainWindow, PatchEditEngine *patch)
    : QGraphicsView()
    , PatchView(patch)
    , mainWindow(mainWindow)
    , moduleBuilder(mainWindow)
    , controllerChooseDialog(mainWindow)
    , dragger(this)
    , previousHeight(0)
{
    setFocusPolicy(Qt::NoFocus);
    setMinimumHeight(RACV_MIN_HEIGHT);
    setMaximumHeight(RACV_MAX_HEIGHT);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *thescene = new QGraphicsScene();
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
    setMouseTracking(true);
    dragRegisterIndicator = new DragRegisterIndicator;
    registerMarker = new RegisterMarker;
    dragControllerIndicator = new DragControllerIndicator;

    CONNECT_ACTION(ACTION_NEW_CONTROLLER, &RackView::addController);
    CONNECT_ACTION(ACTION_TOOLBAR_ADD_CONTROLLER, &RackView::addController);
    CONNECT_ACTION(ACTION_ABORT_ALL_ACTIONS, &RackView::abortAllActions);
    CONNECT_ACTION(ACTION_SHOW_REGISTER_LABELS, &RackView::modifyPatch);
    CONNECT_ACTION(ACTION_SHOW_REGISTER_USAGE, &RackView::modifyPatch);
    CONNECT_ACTION(ACTION_SHOW_USED_G8s, [this]{ showG8s(0); });
    CONNECT_ACTION(ACTION_SHOW_ONE_G8, [this]{ showG8s(1); });
    CONNECT_ACTION(ACTION_SHOW_TWO_G8, [this]{ showG8s(2); });
    CONNECT_ACTION(ACTION_SHOW_THREE_G8,[this]{ showG8s(3); });
    CONNECT_ACTION(ACTION_SHOW_FOUR_G8, [this]{ showG8s(4); });
    CONNECT_ACTION(ACTION_SHOW_X7_ON_DEMAND, &RackView::toggleDisplayOptions);
    CONNECT_ACTION(ACTION_RIGHT_TO_LEFT, &RackView::toggleDisplayOptions);

    connectDragger();

    // Events that we create
    connect(this, &RackView::patchModified, mainWindow->theHub(), &UpdateHub::modifyPatch);
    connect(this, &RackView::sectionSwitched, mainWindow->theHub(), &UpdateHub::sectionSwitched);

    // Events that we are interested in
    connect(mainWindow->theHub(), &UpdateHub::patchModified, this, &RackView::modifyPatch);
    connect(mainWindow->theHub(), &UpdateHub::sectionSwitched, this, &RackView::setRegisterHilitesDirty);
    connect(mainWindow->theHub(), &UpdateHub::cursorMoved, this, &RackView::setRegisterHilitesDirty);

    // Update of register hilites is delayed. This speeds up cursor movement
    registerHilightTimer = new QTimer(this);
    registerHilightTimer->setSingleShot(true);
    connect(registerHilightTimer, &QTimer::timeout, this, &RackView::updateRegisterHilites);

    initScene();
}
void RackView::modifyPatch()
{
    scene()->setBackgroundBrush(COLOR(COLOR_RACK_BACKGROUND));
    dragger.cancel();
    refreshScene();
    setRegisterHilitesDirty();
}
void RackView::toggleDisplayOptions()
{
    QSettings settings;
    settings.setValue("show_x7_on_demand", ACTION(ACTION_SHOW_X7_ON_DEMAND)->isChecked());
    settings.setValue("right_to_left", ACTION(ACTION_RIGHT_TO_LEFT)->isChecked());
    modifyPatch();
    updateSize();
}

void RackView::showG8s(unsigned count)
{
    QSettings settings;
    settings.setValue("show_g8s", count);
    ACTION(ACTION_SHOW_USED_G8s)->setChecked(count == 0);
    ACTION(ACTION_SHOW_ONE_G8)->setChecked(count == 1);
    ACTION(ACTION_SHOW_TWO_G8)->setChecked(count == 2);
    ACTION(ACTION_SHOW_THREE_G8)->setChecked(count == 3);
    ACTION(ACTION_SHOW_FOUR_G8)->setChecked(count == 4);
    modifyPatch();
    updateSize();
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
void RackView::showEvent(QShowEvent *)
{
    updateSize();
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
void RackView::popupModuleContextMenu(int controllerIndex, QString moduleType, AtomRegister areg)
{
    QMenu *menu = new QMenu(this);
    if (controllerIndex >= 0) {
        ADD_ACTION(ACTION_NEW_CONTROLLER, menu);
        menu->addAction(tr("Duplicate this controller"), this,
                        [this,controllerIndex,moduleType] () {this->duplicateController(controllerIndex, false); });
        menu->addAction(tr("Duplicate this controller with labels"), this,
                        [this,controllerIndex,moduleType] () {this->duplicateController(controllerIndex, true); });
        menu->addAction(tr("Remove this controller"), this,
                        [this,controllerIndex,moduleType] () {this->askRemoveController(controllerIndex); });
        menu->addSeparator();
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
                    mainWindow,
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
                    mainWindow,
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
        else if (waitForNext && atom->isRegister() && reg.isRelatedTo(*((AtomRegister *)atom))) {
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
            if (atom->isRegister() && reg.isRelatedTo(*((AtomRegister *)atom))) {
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
void RackView::refreshScene()
{
    setStyleSheet(QString("QGraphicsView { padding: 4px; }"));

    modules.clear();
    if (dragRegisterIndicator->scene())
        scene()->removeItem(dragRegisterIndicator);
    if (registerMarker->scene())
        scene()->removeItem(registerMarker);
    if (dragControllerIndicator->scene())
        scene()->removeItem(dragControllerIndicator);
    scene()->clear();

    scene()->addItem(registerMarker);
    dragRegisterIndicator->setVisible(false);
    scene()->addItem(dragRegisterIndicator);
    dragControllerIndicator->setVisible(false);
    scene()->addItem(dragControllerIndicator);

    x = 0;
    QSettings settings;
    unsigned show_g8s = qMax((unsigned)(settings.value("show_g8s", 0).toInt()),
                             patch->neededG8s());

    if (ACTION(ACTION_RIGHT_TO_LEFT)->isChecked())
    {
        for (int i=patch->numControllers()-1; i>=0; i--)
            addModule(patch->controller(i), i);
        addModule("bling");
        x += RACV_CONTROLLER_GAP;
        if (!ACTION(ACTION_SHOW_X7_ON_DEMAND)->isChecked() || patch->needsX7())
            addModule("x7");
        for (unsigned g=show_g8s; g>=1; g--)
            addModule("g8", -1, g);
        addModule("master");
    }
    else {
        addModule("master");
        for (unsigned g=1; g<=show_g8s; g++)
            addModule("g8", -1, g);
        if (!ACTION(ACTION_SHOW_X7_ON_DEMAND)->isChecked() || patch->needsX7())
            addModule("x7");
        addModule("bling");
        x += RACV_CONTROLLER_GAP;

        for (qsizetype i=0; i<patch->numControllers(); i++)
            addModule(patch->controller(i), i);
    }

    QRectF bounding;
    bounding = QRectF(
                0, 0,
                x,  modules.first()->moduleRect().height());
    scene()->setSceneRect(bounding);

    updateModuleHeights();
}
void RackView::initScene()
{
    refreshScene();
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
    auto sr = scene()->sceneRect();
    double neededWidth = mapFromScene(sr).boundingRect().width();

    // Enable / disable scrollbar with histeresis, to avoid an endless loop.
    // By removing the horizontal scroll bar, we get more height, a resize
    // event and that could lead to having a scroll bar and so on.
    if (previousHeight != 0 && previousHeight < height() && neededWidth > viewport()->width() && horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        return;
    }
    else if (previousHeight > height() && neededWidth + 45 < viewport()->width() && horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        return;
    }

    previousHeight = height();

    if (ACTION(ACTION_RIGHT_TO_LEFT)->isChecked())
        setAlignment(Qt::AlignRight);
    else
        setAlignment(Qt::AlignLeft);

    double availableHeight = viewport()->height();
    resetTransform();
    double s = sr.height() / availableHeight;
    scale(1 / s, 1 / s);

    updateModuleHeights();
}
void RackView::addModule(const QString &name, int controllerIndex, int g8Number)
{
    Module *module = moduleBuilder.buildModule(name, patch->getRegisterLabelsPointer());
    module->setData(DATA_INDEX_MODULE_NAME, name);
    module->setData(DATA_INDEX_MODULE_INDEX, modules.count());
    module->setData(DATA_INDEX_DRAGGER_PRIO, 1);
    scene()->addItem(module);
    modules.append(module);
    if (controllerIndex >= 0)
        module->setData(DATA_INDEX_CONTROLLER_INDEX, controllerIndex);
    if (g8Number > 0)
        module->setData(DATA_INDEX_G8_NUMBER, g8Number);
    module->setPos(x, 0); //RACV_TOP_MARGIN);
    x += module->hp() * RACV_PIXEL_PER_HP + RACK_MODULE_MARGIN;
    module->createRegisterItems(scene(), modules.count() - 1, controllerIndex, g8Number);
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
    QString controller = controllerChooseDialog.chooseController();
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
        shout << "Der name ist " << ar.toString();
        emit registerClicked(ar);
    }
}

void RackView::doubleClickOnItem(QGraphicsItem *item)
{
    if (item->data(DATA_INDEX_MODULE_INDEX).isValid()) {
        Module *module = modules[item->data(DATA_INDEX_MODULE_INDEX).toInt()];
        if (module->getName() == "bling")
            return;
        bool ok;
        int controllerIndex = item->data(DATA_INDEX_CONTROLLER_INDEX).toInt(&ok);
        if (!ok)
            controllerIndex = -1;
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
    Module *module = modules[item->data(DATA_INDEX_MODULE_INDEX).toInt()];
    QString moduleName = module->getName();
    if (moduleName == "bling")
        return;

    QVariant v = item->data(DATA_INDEX_CONTROLLER_INDEX);
    int index = v.isValid() ? v.toInt() : -1;
    AtomRegister areg;
    if (item->data(DATA_INDEX_REGISTER_NAME).isValid())
        areg = item->data(DATA_INDEX_REGISTER_NAME).toString();

    popupModuleContextMenu(index, moduleName, areg);
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

    else if (!startItem->data(DATA_INDEX_REGISTER_NAME).isValid()
          && startItem->data(DATA_INDEX_CONTROLLER_INDEX).isValid())
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
    bool rtl = ACTION(ACTION_RIGHT_TO_LEFT)->isChecked();
    for (auto module: modules) {
        if (module->data(DATA_INDEX_CONTROLLER_INDEX).isValid()) {
            int i = module->data(DATA_INDEX_CONTROLLER_INDEX).toInt();
            if (i == fromIndex)
                continue;
            int snap_distance = RACV_CONTROLLER_SNAP_DISTANCE;
            // Make it easier to drag it to the utter right
            if (i == patch->numControllers() - 1 && x > module->boundingRect().right())
                snap_distance *= 3;

            float left = module->pos().x();
            float right = left + module->moduleRect().width();
            float side_a, side_b;
            if (rtl) {
                side_a = right;
                side_b = left;
            }
            else {
                side_a = left;
                side_b = right;
            }

            if (fromIndex != i-1 && qAbs(side_a - x) < snap_distance) {
                *insertSnap = side_a;
                return i;
            }
            if (fromIndex != i+1 && qAbs(side_b - x) < snap_distance) {
                *insertSnap = side_b;
                return i + 1;
            }
        }
    }
    return -1;
}

void RackView::updateModuleHeights()
{
    for (auto module: modules) {
        module->setPixelHeight(height() - 13); // This 13 is empirical. Other numbers make pixel artefacts
    }
    // This crashes on Qt 6.3.1 if you resize the rack view up and down a lot
    // int padding = height() * RACV_PADDING_MARGIN / 320;
    // setStyleSheet(QString("QGraphicsView { padding-left: %1px; padding-top: %1px; padding-bottom: %2px; background-color: %3; }")
    //               .arg(padding)
    //               .arg(padding - 5)
    //               .arg(COLOR(COLOR_RACK_BACKGROUND).name())
    //               );
}
void RackView::abortDragging()
{
    unsetCursor();
    dragRegisterIndicator->setVisible(false);
    dragControllerIndicator->setVisible(false);
}
void RackView::setRegisterHilitesDirty()
{
    if (!registerHilightTimer->isActive())
        registerHilightTimer->start(RACV_HILIGHT_UPDATE_INTERVAL);
}
void RackView::duplicateController(int controller, bool withLabels)
{
    patch->duplicateController(controller, withLabels);
    patch->commit(tr("duplicating controller"));
    emit patchModified();
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
    ControllerLabellingDialog dialog(mainWindow, labels, moduleType, controllerIndex + 1, reg);
    int ret = dialog.exec();
    if (ret == QDialog::Accepted) {
        patch->commit(tr("editing labelling of '%1'").arg(moduleType));
        emit patchModified();
    }
}
