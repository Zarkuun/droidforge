#include "rackview.h"
#include "colorscheme.h"
#include "controllerlabellingdialog.h"
#include "iconbase.h"
#include "mainwindow.h"
#include "modulebuilder.h"
#include "tuning.h"
#include "controllerchoosedialog.h"
#include "controllerremovaldialog.h"
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
    , dragging(false)
    , draggedAtRegister(false)
{
    setFocusPolicy(Qt::NoFocus);
    setMinimumHeight(RACV_MIN_HEIGHT);
    setMaximumHeight(RACV_MAX_HEIGHT);

    QGraphicsScene *thescene = new QGraphicsScene();
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
    // QPixmap background(":images/background.png");
    // QBrush brush(background.scaledToHeight(1200));
    scene()->setBackgroundBrush(COLOR(COLOR_RACK_BACKGROUND));
    setMouseTracking(true);
    initScene();

    CONNECT_ACTION(ACTION_ADD_CONTROLLER, &RackView::addController);
    CONNECT_ACTION(ACTION_TOOLBAR_ADD_CONTROLLER, &RackView::addController);

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
}

void RackView::resizeEvent(QResizeEvent *)
{
    updateSize();
}

void RackView::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        bool onModule = false;
        for (auto item: items(event->pos())) {
            if (item->data(DATA_INDEX_MODULE_NAME).isValid())
            {
                if (event->button() == Qt::RightButton) {
                    QVariant v = item->data(DATA_INDEX_CONTROLLER_INDEX);
                    int index = v.isValid() ? v.toInt() : -1;
                    popupControllerContextMenu(index, item->data(DATA_INDEX_MODULE_NAME).toString());
                }
                onModule = true;
            }
            else if (item == registerMarker && event->button() == Qt::LeftButton) {
                dragging = true;
                draggedAtRegister = false;
                draggingStartRegister = markedRegister;
                draggingStartPosition = registerMarker->pos();
                maxDistanceFromMouseDown = 0;
                updateDragIndicator(draggingStartPosition, false, false);
                registerMarker->setVisible(false);
            }
        }
        if (!onModule && event->button() == Qt::RightButton)
            popupBackgroundContextMenu();
    }
}

void RackView::mouseReleaseEvent(QMouseEvent *event)
{
     if (dragging) {
         if (maxDistanceFromMouseDown < 100) {
             // Problem: This might be the first of a double click. And
             // double click is labelling. And in that cust we must not
             // emit a registerClicked. Hm. So instead of a single/double
             // click, we make Ctrl-Click for labelling
             if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
                 for (auto item: items(event->pos())) {
                     if (item->data(DATA_INDEX_MODULE_NAME).isValid()) {
                         Module *module = (Module *)item;
                         QVariant v = item->data(DATA_INDEX_CONTROLLER_INDEX);
                         int index = v.isValid() ? v.toInt() : -1;
                         QPointF relPos = mapToScene(event->pos()) - module->pos();
                         AtomRegister *ar = module->registerAt(relPos.toPoint());
                         editLabelling(item->data(DATA_INDEX_MODULE_NAME).toString(), index, ar ? *ar : AtomRegister());
                         break;
                     }
                 }
             }
             else {
                 emit registerClicked(draggingStartRegister);
                 dragRegisterIndicator->setVisible(false);
             }
         }
         else {
             AtomRegister draggingEndRegister = markedRegister;
             if (registersSuitableForSwapping(draggingStartRegister, draggingEndRegister)) {
                 swapRegisters(draggingStartRegister, draggingEndRegister);
                 dragRegisterIndicator->doSuccessAnimation();
             }
             else
                 dragRegisterIndicator->setVisible(false);
         }

         registerMarker->setVisible(true);
         dragging = false;
         markedRegister = AtomRegister();
         scene()->update();
     }
}

void RackView::mouseDoubleClickEvent(QMouseEvent *event)
{
    bool foundItem = !items(event->pos()).empty();
    if (!foundItem)
        TRIGGER_ACTION(ACTION_ADD_CONTROLLER);
}

void RackView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos(); // mapToScene(event->pos()).toPoint();
    if (dragging) {
        QPointF vector = draggingStartPosition - mapToScene(mousePos);
        maxDistanceFromMouseDown = qMax(maxDistanceFromMouseDown, vector.manhattanLength());
    }

    bool foundRegister = false;
    for (auto item: items(mousePos))
    {
        if (item->data(DATA_INDEX_MODULE_NAME).isValid()) {
            Module *module = (Module *)item;
            QPointF relPos = mapToScene(mousePos) - module->pos();
            AtomRegister *ar = module->registerAt(relPos.toPoint());
            if (ar != 0)
            {
                foundRegister = true;
                QChar t = ar->getRegisterType();
                unsigned n = ar->number() - module->numberOffset(t);
                QPointF pos = module->registerPosition(t, n) * RACV_PIXEL_PER_HP;
                float diameter = module->registerSize(t, n) * RACV_PIXEL_PER_HP;
                if (dragging && (!draggedAtRegister || markedRegister != *ar))
                {
                    QPointF center = pos + module->pos();
                    bool suitable = registersSuitableForSwapping(*ar, draggingStartRegister);
                    updateDragIndicator(center, true, suitable);
                    draggedAtRegister = true;
                    diameter = RACV_PIXEL_PER_HP;
                }
                if  (markedRegister != *ar)
                {
                    markedRegister = *ar;
                    if (!dragging)
                        updateRegisterMarker(pos + module->pos(), diameter);
                }
                delete ar;
            }
            else if (!ar) {
                markedRegister = AtomRegister(0, 0, 0);
                registerMarker->setVisible(false);
            }
            break;
        }
    }

    if (dragging && !foundRegister) {
        draggedAtRegister = false;
        QPointF end = mapToScene(event->pos());
        updateDragIndicator(end, false, false);
    }
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

void RackView::updateRegisterMarker(QPointF p, float diameter)
{
    diameter += RACV_REGMARKER_EXTRA_DIAMETER;
    registerMarker->setPos(p);
    registerMarker->setDiameter(diameter);
    registerMarker->setVisible(true);
    registerMarker->startAnimation();
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
            if (candidate.getController() == controller)
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
}

void RackView::popupControllerContextMenu(int controllerIndex, QString moduleType)
{
   QMenu *menu = new QMenu(this);
   if (controllerIndex >= 0) {
       ADD_ACTION(ACTION_ADD_CONTROLLER, menu);
       menu->addAction(tr("Remove this controller"), this,
                       [this,controllerIndex,moduleType] () {this->askRemoveController(moduleType, controllerIndex); });
       if (controllerIndex > 0)
           menu->addAction(tr("Move by one position to the left"), this,
                           [this,controllerIndex] () {this->moveController(controllerIndex, controllerIndex-1); });
       if (controllerIndex+1 < patch->numControllers())
           menu->addAction(tr("Move by one position to the right"), this,
                           [this,controllerIndex] () {this->moveController(controllerIndex, controllerIndex+1); });
       if (controllersRegistersUsed(controllerIndex) && numControllers() >= 2)
           menu->addAction(tr("Move used controls and LEDs to other controllers"),
                           this, [this,controllerIndex,moduleType] () {this->remapControls(moduleType, controllerIndex); });
   }

   AtomRegister reg = markedRegister;
   menu->addAction(tr("Edit labelling of controls"), this,
                   [this,controllerIndex,moduleType,reg] () {this->editLabelling(moduleType, controllerIndex, reg); });

   if (!markedRegister.isNull() && patch->registerUsed(markedRegister)) {
       menu->addAction(tr("Find this register in your patch"), this,
                       [this,reg] () {this->findRegister(reg); });
   }

   menu->addSeparator();

   menu->addAction(tr("Lookup this module in the shop"), this,
                   [this,moduleType] () {this->purchaseController(moduleType); });
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->popup(QCursor::pos());
}

void RackView::popupBackgroundContextMenu()
{
   QMenu *menu = new QMenu(this);
   ADD_ACTION(ACTION_ADD_CONTROLLER, menu);
   menu->popup(QCursor::pos());
}

void RackView::askRemoveController(const QString moduleType, int controllerIndex)
{
    // Get a list of all registers that are in use
    RegisterList atomsToRemap;
    collectUsedRegisters(controllerIndex, atomsToRemap);
    if (atomsToRemap.empty())
        removeController(controllerIndex, moduleType, atomsToRemap);

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
            removeController(controllerIndex, moduleType, atomsToRemap, ih, oh);
        }
    }
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

void RackView::updateDragIndicator(QPointF endPos, bool hits, bool suitable)
{
    dragRegisterIndicator->abortAnimation();
    dragRegisterIndicator->setPos(draggingStartPosition);
    dragRegisterIndicator->setEnd(endPos - draggingStartPosition, hits, suitable);
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

    // TODO: Vereinheitlichen mit followCable()
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

void RackView::moveController(int fromindex, int toindex)
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
        if (item->data(DATA_INDEX_MODULE_NAME).isValid())
            scene()->removeItem(item);
    }
    x = 0;
    addModule("master");
    // if (patch->needG8())
        addModule("g8");
    // if (patch->needX7())
        addModule("x7");
    // addModule("blind");
    x += RACV_CONTROLLER_GAP;

    for (qsizetype i=0; i<patch->numControllers(); i++)
        addModule(patch->controller(i), i);

    QPointF margin(RACV_MAIN_MARGIN, RACV_MAIN_MARGIN);
    QRectF bounding(
                modules.first()->boundingRect().topLeft() - margin,
                modules.last()->boundingRect().bottomRight());
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

    // Add strut, so space above and below the modules is visible
    // scene()->addLine(0, 0, 0, RACV_BACKGROUND_HEIGHT, QPen(QColor(0, 0, 0, 0)));
    // TODO: kann man das nicht mit setSceneRect() besser machen?
    refreshModules();
    updateSize();
}

void RackView::updateSize()
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void RackView::addModule(const QString &name, int controllerIndex)
{
    Module *module = ModuleBuilder::buildModule(name, patch->getRegisterLabelsPointer());
    module->setData(DATA_INDEX_MODULE_NAME, name);
    scene()->addItem(module);
    modules.append(module);
    if (controllerIndex >= 0)
        module->setData(DATA_INDEX_CONTROLLER_INDEX, controllerIndex);
    module->setPos(x, 0); //RACV_TOP_MARGIN);
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

void RackView::updateRegisterHilites()
{
    const Circuit *circuit = section()->currentCircuit();
    if (!circuit)
        return;

    RegisterList currentRegisters;
    CursorPosition cursor = section()->cursorPosition();
    if (cursor.row == -2 || cursor.row == -1) // Circuit selected
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

    RegisterList usedRegisters;
    patch->collectUsedRegisterAtoms(usedRegisters);

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

void RackView::remapControls(QString controllerName, int controllerIndex)
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
