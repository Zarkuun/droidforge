#include "patchsectionview.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomoneliner.h"
#include "atomregister.h"
#include "circuitview.h"
#include "cablecolorizer.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "mainwindow.h"
#include "tuning.h"
#include "commentdialog.h"
#include "circuitchoosedialog.h"
#include "patchview.h"
#include "namechoosedialog.h"
#include "problemmarker.h"
#include "circuitchoosedialog.h"
#include "updatehub.h"
#include "jackchoosedialog.h"
#include "patchoperator.h"

#include <QMouseEvent>
#include <QGraphicsProxyWidget>
#include <QTransform>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

#define DATA_INDEX_CIRCUIT 0
#define DATA_INDEX_PROBLEM 1


PatchSectionView::PatchSectionView(VersionedPatch *initialPatch)
    : PatchOperator(initialPatch) // patch is never ever 0!
    , zoomLevel(0)
    , zoomFactor(1.0)
    , atomSelectorDialog{}
    , selection(0)
{
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    buildPatchSection();
    QPixmap background(":images/background.png");
    setBackgroundBrush(QBrush(background.scaledToHeight(BACKGROUND_PIXMAP_HEIGHT)));

    QSettings settings;
    if (settings.contains("patchwindow/zoom"))
        zoomLevel = settings.value("patchwindow/zoom").toInt();
    setZoom(zoomLevel);

    connectActions();

    // Events that we create
    connect(this, &PatchSectionView::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchSectionView::clipboardChanged, the_hub, &UpdateHub::changeClipboard);
    connect(this, &PatchSectionView::selectionChanged, the_hub, &UpdateHub::changeSelection);
    connect(this, &PatchSectionView::cursorMoved, the_hub, &UpdateHub::moveCursor);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &PatchSectionView::switchSection);
    connect(the_hub, &UpdateHub::patchModified, this, &PatchSectionView::modifyPatch);
    connect(the_hub, &UpdateHub::selectionChanged, this, &PatchSectionView::changeSelection);
    connect(the_hub, &UpdateHub::cursorMoved, this, &PatchSectionView::moveCursor);
}

PatchSectionView::~PatchSectionView()
{
    deletePatchSection();
    if (atomSelectorDialog)
        delete atomSelectorDialog;
}

void PatchSectionView::connectActions()
{
    CONNECT_ACTION(ACTION_CUT, &PatchSectionView::cut);
    CONNECT_ACTION(ACTION_COPY, &PatchSectionView::copy);
    CONNECT_ACTION(ACTION_PASTE, &PatchSectionView::paste);
    CONNECT_ACTION(ACTION_PASTE_SMART, &PatchSectionView::pasteSmart);
    CONNECT_ACTION(ACTION_CREATE_SECTION_FROM_SELECTION, &PatchSectionView::createSectionFromSelection);
    CONNECT_ACTION(ACTION_NEW_CIRCUIT, &PatchSectionView::newCircuit);
    CONNECT_ACTION(ACTION_ADD_JACK, &PatchSectionView::addJack);
    CONNECT_ACTION(ACTION_EDIT_VALUE, &PatchSectionView::editValue);
    CONNECT_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, &PatchSectionView::editCircuitComment);
    CONNECT_ACTION(ACTION_RENAME_CABLE, &PatchSectionView::renameCable);
    CONNECT_ACTION(ACTION_RESET_ZOOM, &PatchSectionView::zoomReset);
    CONNECT_ACTION(ACTION_ZOOM_IN, &PatchSectionView::zoomIn);
    CONNECT_ACTION(ACTION_ZOOM_OUT, &PatchSectionView::zoomOut);
}

void PatchSectionView::buildPatchSection()
{
    unsigned circuitWidth = viewport()->width() / zoomFactor;
    QGraphicsScene *scene = new QGraphicsScene();
    setScene(scene);

    int y = 0;
    for (qsizetype i=0; i<section()->circuits.size(); i++)
    {
        bool isLast = i == section()->circuits.size() - 1;
        Circuit *circuit = section()->circuits[i];
        CircuitView *cv = new CircuitView(
                    circuit,
                    i,
                    &selection,
                    circuitWidth,
                    fontMetrics().lineSpacing(),
                    isLast ? CIRV_TOP_PADDING : 0);
        cv->setData(DATA_INDEX_CIRCUIT, true);
        circuitViews.append(cv);
        scene->addItem(cv);
        cv->setPos(0, y); // TODO: der erste parameter wirkt nicht
        y += cv->boundingRect().height();
    }
    scene->addItem(&frameCursor);
    updateCursor();
}

void PatchSectionView::updateProblemMarkers()
{
    qDebug() << "WIEDER AKTIVIEREN";
    return;

    for (auto item: scene()->items()) {
        if (item->data(DATA_INDEX_PROBLEM).isValid()) {
            scene()->removeItem(item);
        }
    }

    for (auto problem: patch->allProblems())
    {
        if (problem->getSection() == patch->currentSectionIndex()) { // TODO: HACK!!
            const CursorPosition &pos = problem->getCursorPosition();
            CircuitView *cv = circuitViews[pos.circuitNr];
            QRectF rect = cv->cellRect(pos.row, pos.column);
            ProblemMarker *marker = new ProblemMarker(rect.height(), problem->getReason());
            scene()->addItem(marker);
            QPointF p(cv->pos().x() + rect.right() - rect.height(),
                      cv->pos().y() + rect.top());
            marker->setPos(p);
            marker->setData(DATA_INDEX_PROBLEM, true);
        }
    }
}

void PatchSectionView::deletePatchSection()
{
    for (unsigned i=0; i<circuitViews.size(); i++)
        delete circuitViews[i];
    circuitViews.clear();
}

void PatchSectionView::rebuildPatchSection()
{
    deletePatchSection();
    buildPatchSection();
    updateProblemMarkers();
    // TODO: Scheinbar springt der Viewport hier immer nach oben.
    // Und das ensureVisible bewirkt erstmal nix bis man den Cursor
    // bewegt
}

bool PatchSectionView::handleKeyPress(QKeyEvent *event)
{
    qDebug() << "KEY" << event;
    int key = event->key();
    bool shiftHeld = event->modifiers() & Qt::ShiftModifier;

    CursorPosition posBefore = section()->cursorPosition();
    bool moved = false;

    switch (key) {
    case Qt::Key_Up:        moveCursorUpDown(-1);     moved = true; break;
    case Qt::Key_Down:      moveCursorUpDown(1);      moved = true; break;
    case Qt::Key_Left:      moveCursorLeftRight(-1);  moved = true; break;
    case Qt::Key_Right:     moveCursorLeftRight(1);   moved = true; break;
    case Qt::Key_PageUp:    moveCursorPageUpDown(-1); moved = true; break;
    case Qt::Key_PageDown:  moveCursorPageUpDown(1);  moved = true; break;
    case Qt::Key_Backspace: deleteCursorOrSelection(); return true; // TODO
    }

    if (moved) {
        if (shiftHeld)
            updateKeyboardSelection(posBefore, section()->cursorPosition());
        else
            clearSelection();
        return true;
    }

    // All keys that are used for entering a value popup the atom selector
    // preselected accordingly.
    if ( (key >= Qt::Key_A && key <= Qt::Key_Z)
        || key == Qt::Key_Underscore
        || (key >= Qt::Key_0 && key <= Qt::Key_9)
         || key == Qt::Key_Period
         || key == Qt::Key_Minus)
    {
        editValue(key);
        return true;
    }
    return true;
}

void PatchSectionView::mousePressEvent(QMouseEvent *event)
{
    mouseClick(event->pos(), event->button(), false);
}

void PatchSectionView::mouseDoubleClickEvent(QMouseEvent *event)
{
    mouseClick(event->pos(), event->button(), true);
}

void PatchSectionView::mouseClick(QPoint pos, int button, bool doubleClick)
{
    // itemAt() applies the transformation of the graphics
    // view such as the scroll bar and the alignment.

    CircuitView *cv = 0;
    for (auto item: items(pos)) {
        if (item->data(DATA_INDEX_CIRCUIT).isValid()) {
            cv = (CircuitView *)item;
            break;
        }
    }

    if (cv)
    {
        QPointF posInScene(mapToScene(pos));
        QPointF posInCircuit = posInScene - cv->pos();

        // Loop over all circuits because we need the index number
        // of the clicked circuit, not just the pointer.
        for (unsigned i=0; i<circuitViews.size(); i++)
        {
            if (circuitViews[i] == cv) {
                CursorPosition curPos;
                curPos.circuitNr = i;
                curPos.row = cv->jackAt(posInCircuit.y());
                curPos.column = cv->columnAt(posInCircuit.x());

                if (doubleClick)
                    editValueByMouse(curPos);
                else if (button == Qt::LeftButton)
                    handleLeftMousePress(curPos);
                else
                    handleRightMousePress(cv, curPos);
                return;
            }
        }
    }

    // click on background
    // TODO: The new circuit should appear at the end of the
    // section, not where the cursor is.
    if (button == Qt::RightButton)
        handleRightMousePress(0, CursorPosition());
}

void PatchSectionView::changeSelection(const Selection *)
{
    rebuildPatchSection();
}

void PatchSectionView::modifyPatch()
{
    // We are responsible for the unique and complete color decision
    // of all internal cables.
    the_cable_colorizer->colorizeAllCables(patch->allCables());
    rebuildPatchSection();
}

void PatchSectionView::switchSection()
{
    rebuildPatchSection();
}

void PatchSectionView::moveCursor()
{
    updateCursor();
}

void PatchSectionView::newCircuit()
{
    jackselection_t jackSelection;
    QString name = CircuitChooseDialog::chooseCircuit(jackSelection);
    if (name != "") {
        section()->addNewCircuit(name, jackSelection);
        patch->commit(tr("adding new '%1' circuit").arg(name));
        emit patchModified();
    }
}

void PatchSectionView::addJack()
{
    if (section()->isEmpty())
        return;

    QString name = JackChooseDialog::chooseJack(currentCircuitName(), "", usedJacks());
    if (name == "")
        return;

    int row = section()->cursorPosition().row;
    int index = row + 1;
    if (index < 0)
        index = 0;

    currentCircuit()->insertJackAssignment(buildJackAssignment(name), index);
    section()->setCursorRow(index);
    section()->setCursorColumn(1);
    patch->commit(tr("adding new jack '%1'").arg(name));
    emit patchModified();
}

void PatchSectionView::cut()
{
    copyToClipboard();
    deleteCursorOrSelection();
}

void PatchSectionView::copy()
{
    copyToClipboard();
}

void PatchSectionView::paste()
{
    if (the_clipboard->numCircuits()) {
        pasteCircuitsFromClipboard();
        qDebug() << "TODO: Hier Dialog wegen anpassen der Register!";
    }
    else if (isEmpty()) {
        // TODO: updateActions in mainwindow soll paste deaktivieren, wenn
        // dieser Fall da ist.
        return;
    }
    else if (the_clipboard->isComment())
        pasteCommentFromClipboard();
    else if (the_clipboard->numJacks())
        pasteJacksFromClipboard();
    else if (the_clipboard->numAtoms())
        pasteAtomsFromClipboard();
    else {
        // Should never happen
    }
}

void PatchSectionView::pasteSmart()
{
    Patch *pastedPatch = the_clipboard->getAsPatch();
    if (!interactivelyRemapRegisters(pastedPatch)) {
        delete pastedPatch;
        return;
    }

    int position = 0;
    for (auto circuit: pastedPatch->section(0)->getCircuits()) {
        Circuit *newCircuit = circuit->clone();
        position = section()->isEmpty() ? 0 : section()->cursorPosition().circuitNr + 1;
        section()->addCircuit(position, newCircuit);
    }
    section()->setCursor(CursorPosition(position, -2, 0));
    patch->commit(tr("smart pasting %1 circuits").arg(the_clipboard->getCircuits().count()));
    emit patchModified();
}

void PatchSectionView::pasteCircuitsFromClipboard()
{
    int position = 0;
    for (auto circuit: the_clipboard->getCircuits()) {
        Circuit *newCircuit = circuit->clone();
        position = section()->isEmpty() ? 0 : section()->cursorPosition().circuitNr + 1;
        section()->addCircuit(position, newCircuit);
    }
    section()->setCursor(CursorPosition(position, -2, 0));
    patch->commit(tr("pasting %1 circuits").arg(the_clipboard->getCircuits().count()));
    emit patchModified();
}

void PatchSectionView::pasteCommentFromClipboard()
{
    QString comment = the_clipboard->getComment();
    if (comment != currentCircuit()->getComment()) {
        currentCircuit()->setComment(comment);
        patch->commit(tr("pasting circuit comment"));
        emit patchModified();
    }
}

void PatchSectionView::pasteJacksFromClipboard()
{
    const QList<JackAssignment *> &jas = the_clipboard->getJackAssignment();
    Circuit *circuit = currentCircuit();
    int index = qMin(circuit->numJackAssignments(), qMax(0, section()->cursorPosition().row + 1));
    for (auto ja: jas) {
        // We need to reparse the jack assignment, because the circuit we
        // paste into is maybe another circuit and e.g. "clock" might have
        // to change from an input to an output.
        QString asString = ja->toString();
        JackAssignment *jaReparsed = JackAssignment::parseJackLine(circuit->getName(), asString);
        circuit->insertJackAssignment(jaReparsed, index);
        section()->setCursorRow(index);
        index++;
    }
    patch->commit(tr("pasting %1 jack assignments").arg(jas.count()));
    emit patchModified();
}

void PatchSectionView::pasteAtomsFromClipboard()
{
    JackAssignment *ja = currentJackAssignment();
    Q_ASSERT(ja);

    int column = section()->cursorPosition().column;
    Q_ASSERT(column >= 1 && column <= 3);

    const QList<Atom *> &atoms = the_clipboard->getAtoms();
    for (auto atom: atoms) {
        // We need to reparse, as people might copy an input
        // value like "17.4" to an output parameter.
        Atom *newAtom = 0;
        if (atom) {
            QString asString = atom->toString();
            if (ja->isOutput())
                newAtom = JackAssignmentOutput::parseOutputAtom(asString);
            else if (ja->isInput())
                newAtom = JackAssignmentInput::parseInputAtom(asString);
            else
                newAtom = new AtomInvalid(asString);
        }
        ja->replaceAtom(column, newAtom);
        section()->setCursorColumn(column);
        column ++;
        if (column > 3)
            break;
    }
    patch->commit(tr("pasting parameters"));
    emit patchModified();
}

void PatchSectionView::zoomReset()
{
    changeZoom(0);
}

void PatchSectionView::zoomIn()
{
    changeZoom(1);
}

void PatchSectionView::zoomOut()
{
    changeZoom(-1);
}

void PatchSectionView::changeZoom(int how)
{
    if (how == 0)
        zoomLevel = 0;
    else
        zoomLevel += how;
    zoomLevel = qMin(ZOOM_MAX, qMax(ZOOM_MIN, zoomLevel));
    QSettings settings;
    settings.setValue("patchwindow/zoom", zoomLevel);
    setZoom(zoomLevel);
}

void PatchSectionView::setZoom(int zoom)
{
    zoomFactor = pow(ZOOM_STEP, double(zoom));
    QTransform transform;
    transform.scale(zoomFactor, zoomFactor);
    setTransform(transform);
    rebuildPatchSection();
    setMinimumWidth(CircuitView::minimumWidth() * zoomFactor + ASSUMED_SCROLLBAR_WIDTH);
}

void PatchSectionView::handleLeftMousePress(const CursorPosition &curPos)
{
    if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier) {
        setMouseSelection(curPos);
        emit selectionChanged(selection);
    }
    else {
        clearSelection();
        section()->setCursor(curPos);
        emit cursorMoved();
    }
}

void PatchSectionView::handleRightMousePress(CircuitView *cv, const CursorPosition &curPos)
{
    // TODO: Show a different menu when a selection is active and
    // a cell from the selection is being hit?
    if (selection) {
        return;
    }

    // Make sure that cursor is set to the cell the menu is
    // working with. Otherwise all actions would address the
    // wrong cell.
    if (cv) {
        section()->setCursor(curPos);
        emit cursorMoved();
    }

    QMenu *menu = new QMenu(this);
    ADD_ACTION(ACTION_NEW_CIRCUIT, menu);
    //menu->addAction(the_forge->action(ACTION_NEW_CIRCUIT));
    if (cv) {
        ADD_ACTION(ACTION_EDIT_VALUE, menu);
        ADD_ACTION(ACTION_ADD_JACK, menu);
        // TOOD:
        // - remove circut
        // - remove comment
        // ..
        if (curPos.row >= 0) {
            if (curPos.column == 0) {
                // TODO:
                // - delete jack assignment
            }
            else {
                menu->addSeparator();
                const Atom *atom = currentAtom();
                if (atom && atom->isCable()) {
                    ADD_ACTION(ACTION_FOLLOW_INTERNAL_CABLE, menu);
                    ADD_ACTION(ACTION_RENAME_CABLE, menu);
                }
                ADD_ACTION(ACTION_START_PATCHING, menu);
                ADD_ACTION(ACTION_FINISH_PATCHING, menu);
                ADD_ACTION(ACTION_ABORT_PATCHING, menu);
            }
        }
    }

    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->popup(QCursor::pos());
}

void PatchSectionView::resizeEvent(QResizeEvent *)
{
    rebuildPatchSection();
    updateCursor();
}

void PatchSectionView::showEvent(QShowEvent *)
{
    // updateCableIndicator(); Was macht das Hier/??
}

PatchView *PatchSectionView::patchView()
{
    // TODO: Das hier ist der Megahack.
    return (PatchView *)parent()->parent();
}

JackAssignment *PatchSectionView::buildJackAssignment(const QString &name)
{
    Circuit *circuit = currentCircuit();
    QString circuitName = circuit->getName();
    if (the_firmware->jackIsInput(circuitName, name))
        return new JackAssignmentInput(name);
    else if (the_firmware->jackIsOutput(circuitName, name))
        return new JackAssignmentOutput(name);
    else
        return new JackAssignmentUnknown(name);
}

QChar PatchSectionView::keyToChar(int key)
{
    if (key >= 0 && key <= 127)
        return QChar(key);
    else
        return ' ';
}

void PatchSectionView::copyToClipboard()
{
    if (selection) {
        the_clipboard->copyFromSelection(selection, section());
    }
    else {
        Selection sel(section()->cursorPosition());
        the_clipboard->copyFromSelection(&sel, section());
    }
    emit clipboardChanged();
}

Patch *PatchSectionView::getSelectionAsPatch() const
{
    Clipboard cb;
    cb.copyFromSelection(selection, section());
    return cb.getAsPatch();
}

void PatchSectionView::createSectionFromSelection()
{
    QString newname = NameChooseDialog::getName(tr("Create new section from selection"), tr("New name:"));
    if (newname.isEmpty())
        return;
    Clipboard cb;
    cb.copyFromSelection(selection, patch->currentSection());
    deleteCursorOrSelection();
    PatchSection *newSection = new PatchSection(newname);
    for (auto circuit: cb.getCircuits())
        newSection->addCircuit(circuit->clone());
    int index = patch->currentSectionIndex() + 1;
    patch->insertSection(index, newSection);
    patch->switchCurrentSection(index);
    patch->commit(tr("moving circuits into new section"));
    emit patchModified();
}

void PatchSectionView::editJack(int key)
{
    if (key)
        return; // direct editing currently not implemented

    JackAssignment *ja = currentJackAssignment();

    QString name = JackChooseDialog::chooseJack(
                currentCircuitName(),
                ja->jackName(),
                usedJacks(),
                ja->jackType());

    if (!name.isEmpty()) {
        // When we convert formerly unknown jacks to known jacks,
        // we need to reparse the still unparsed value expression
        if (ja->jackType() == JACKTYPE_UNKNOWN) {
            JackAssignment *newJa = buildJackAssignment(name);
            newJa->parseExpression(ja->valueToString());
            section()->deleteCurrentJackAssignment();
            currentCircuit()->insertJackAssignment(newJa, section()->cursorPosition().row);
        }
        else
            currentJackAssignment()->changeJack(name);
        patch->commit(tr("replacing jack with '%s'").arg(name));
        emit patchModified();
    }
}


QString PatchSectionView::currentCircuitName() const
{
    // TODO: Wenn es keinen current circuit gibt.
    return currentCircuit()->getName();
}

QStringList PatchSectionView::usedJacks() const
{
    return currentCircuitView()->usedJacks();
}


void PatchSectionView::editValue(int key)
{
    if (isEmpty())
        return;

    int row = section()->cursorPosition().row;
    int column = section()->cursorPosition().column;

    if (row == -2)
        editCircuit(key);
    else if (row == -1)
        editCircuitComment(key);
    else if (column == 0)
        editJack(key);
    else
        editAtom(key);
}


void PatchSectionView::editValueByMouse(CursorPosition &pos)
{
    section()->setCursor(pos);
    emit cursorMoved();
    editValue(0);
}


void PatchSectionView::editAtom(int key)
{
    // if (key == 0 && patchView()->isPatching())  {
    //     patchView()->finishPatching();
    //     return;
    // }

    Circuit *circuit = currentCircuit();
    JackAssignment *ja = circuit->jackAssignment(section()->cursorPosition().row);
    if (ja->jackType() == JACKTYPE_UNKNOWN)
        return; // TODO: Edit unknown data anyway?

    CursorPosition curPos = section()->cursorPosition();
    const Atom *atom = ja->atomAt(curPos.column);
    Atom *newAtom;

    if (key != 0) {
        QRectF cursor = currentCircuitView()->cellRect(curPos.row, curPos.column);
        QPointF posRelativeToScene = cursor.topLeft();
        QPoint posRelativeToView = mapFromScene(posRelativeToScene);
        QPoint posInScreen = mapToGlobal(posRelativeToView);
        QChar c(key);
        QString start(c);
        newAtom = AtomOneliner::editAtom(posInScreen, patch, ja->jackType(), start);
    }
    else
        newAtom = AtomSelectorDialog::editAtom(patch, ja->jackType(), curPos.column == 2, atom);

    if (newAtom != 0 && newAtom != atom) {
        ja->replaceAtom(section()->cursorPosition().column, newAtom);
        patch->commit(tr("changing parameter '%1'").arg(ja->jackName()));
        emit patchModified();
    }
}

void PatchSectionView::editCircuitComment(int key)
{
    Circuit *circuit = currentCircuit();

    QString oldComment;
    if (key)
        oldComment = QString(QChar(key));
    else
        oldComment = circuit->getComment();
    QString newComment = CommentDialog::editComment(oldComment);
    if (newComment != oldComment) {
        if (newComment != "")
            circuit->setComment(newComment);
        else
            circuit->removeComment();
        section()->setCursorRow(-1);
        patch->commit(tr("changing comment for circuit '%1'").arg(circuit->getName()));
        emit patchModified();
    }
}

void PatchSectionView::renameCable()
{
    const Atom *atom = currentAtom();
    if (!atom || !atom->isCable())
        return;

    // TODO: Automatisch immer groß schreiben der Kabel
    QString oldName = ((AtomCable *)atom)->getCable();
    QString newName = NameChooseDialog::getName(
                tr("Rename internal cable '%1'").arg(oldName),
                tr("New name:"),
                oldName);
    if (newName == oldName)
        return;

    newName = newName.toUpper();
    QStringList all = patch->allCables();
    if (all.contains(newName)) {
        int reply = QMessageBox::warning(
                    this,
                    tr("Conflict"),
                    tr("There already is a cable with this name. Choosing the name '%1'"
                       "will join these cables and make all according inputs and outputs "
                       "connected.\n\n"
                       "Do you want want to join both cables?").arg(newName),
                    QMessageBox::Cancel | QMessageBox::Yes,
                    QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel)
            return;
    }

    patch->renameCable(oldName, newName);
    patch->commit(tr("renaming cable '%1' to '%2'").arg(oldName).arg(newName));
    emit patchModified();
}

bool PatchSectionView::isEmpty() const
{
    return section()->circuits.empty();
}

bool PatchSectionView::circuitsSelected() const
{
    return selection && selection->isCircuitSelection();
}

void PatchSectionView::updateCircuits()
{
    for (unsigned i=0; i<circuitViews.size(); i++)
        circuitViews[i]->update();
}

void PatchSectionView::updateRegisterHilites() const
{
    const Circuit *circuit = currentCircuit();
    if (!currentCircuit()) // 0 for empty section
        return;

    CursorPosition cursor = section()->cursorPosition();
    RegisterList registers;
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
    // TODO:
    // registers.removeDuplicates();
    the_forge->hiliteRegisters(registers);
}

void PatchSectionView::clickOnRegister(AtomRegister ar)
{
    CursorPosition cursor = section()->cursorPosition();
    if (cursor.row < 0 || cursor.column == 0) return;
    JackAssignment *ja = currentCircuit()->jackAssignment(cursor.row);

    // This is a bit of a hack, but I'm not sure how to do
    // this in a much more clean way. If the user e.g. clicks
    // on I5 but is just editing on output jack, he rather
    // wants N5. Same is for buttons and LEDs. In the rare
    // case of using LEDs as input, clicking simply does not
    // work. The user has to select the register by other means.
    // So what...
    if (ja->isOutput()) {
        if (ar.getRegisterType() == REGISTER_INPUT)
            ar.setRegisterType(REGISTER_NORMALIZE);
        else if (ar.getRegisterType() == REGISTER_BUTTON)
            ar.setRegisterType(REGISTER_LED);
    }

    ja->replaceAtom(cursor.column, ar.clone());
    patch->commit(tr("inserting register %1").arg(ar.toString()));
    emit patchModified();
}

void PatchSectionView::setCursorMode(cursor_mode_t mode)
{
    frameCursor.setMode(mode);
}

// Repositions the frameCursor so that it matches the current
// cursor position in the current section
void PatchSectionView::updateCursor()
{
    if (currentCircuitView()) {
        const CursorPosition &pos = section()->cursorPosition();
        QRectF br = currentCircuitView()->boundingRect();
        QRectF tbr = br.translated(currentCircuitView()->pos());
        ensureVisible(tbr); // TODO: Das hier geht noch nicht so gut

        QRectF cr = currentCircuitView()->cellRect(pos.row, pos.column);
        frameCursor.setVisible(true);
        frameCursor.setRect(cr.translated(currentCircuitView()->pos()));
        frameCursor.startAnimation();
    }
    else
        frameCursor.setVisible(false);
}

void PatchSectionView::setMouseSelection(const CursorPosition &to)
{
    if (selection)
        delete selection;
    selection = new Selection(section()->cursorPosition(), to);
    emit selectionChanged(selection);
}

void PatchSectionView::updateKeyboardSelection(const CursorPosition &before, const CursorPosition &after)
{
    if (selection) {
        CursorPosition from, to;
        if (selection->fromPos() == before) {
            from = after;
            to = selection->toPos();
        }
        else {
            from = selection->fromPos();
            to = after;
        }
        delete selection;
        selection = new Selection(from, to);
    }
    else
        selection = new Selection(before, after);
    emit selectionChanged(selection);
}

void PatchSectionView::clearSelection()
{
    if (selection) {
        delete selection;
        selection = 0;
        emit selectionChanged(selection);
    }
}

CircuitView *PatchSectionView::currentCircuitView()
{
    if (circuitViews.isEmpty())
        return 0;
    else
        return circuitViews[section()->cursorPosition().circuitNr];
}

const CircuitView *PatchSectionView::currentCircuitView() const
{
    if (circuitViews.isEmpty())
        return 0;
    else
        return circuitViews[section()->cursorPosition().circuitNr];
}


Circuit *PatchSectionView::currentCircuit()
{
    return section() ? section()->currentCircuit() : 0;

}

const Circuit *PatchSectionView::currentCircuit() const
{
    return section() ? section()->currentCircuit() : 0;
}


JackAssignment *PatchSectionView::currentJackAssignment()
{
    return section()->currentJackAssignment();
}

void PatchSectionView::moveCursorPageUpDown(int whence)
{
    if (whence == -1)
        section()->moveCursorToPreviousCircuit();
    else
        section()->moveCursorToNextCircuit();
    emit cursorMoved();
}

void PatchSectionView::deleteCursorOrSelection()
{
    if (selection) {
        // When just a single object is selected, we can use
        // the existing functions for deleting at the cursor position
        if (selection->isCommentSelection())
            deleteCurrentComment(); // cursor must be in selection
        else if (selection->isSingleCircuitSelection())
            deleteCurrentCircuit();
        else if (selection->isSingleJackSelection())
            deleteCurrentJack();
        else if (selection->isSingleAtomSelection())
            deleteCurrentAtom();

        // Multiple selection
        else if (selection->isCircuitSelection())
            deleteMultipleCircuits(selection->fromPos().circuitNr,
                           selection->toPos().circuitNr);
        else if (selection->isJackSelection())
            deleteMultipleJacks(selection->fromPos().circuitNr,
                                selection->fromPos().row,
                                selection->toPos().row);
        else if (selection->isAtomSelection())
            deleteMultipleAtoms(selection->fromPos().circuitNr,
                                selection->fromPos().row,
                                selection->fromPos().column,
                                selection->toPos().column);
    }
    else {
        deleteCurrentRow();
    }
    clearSelection();
}


void PatchSectionView::deleteCurrentRow()
{
    if (isEmpty())
        return;

    const CursorPosition &pos = section()->cursorPosition();
    if (pos.row == -2)
        deleteCurrentCircuit();
    else if (pos.row == -1)
        deleteCurrentComment();
    else if (pos.column == 0)
        deleteCurrentJack();
    else
        deleteCurrentAtom();
}

void PatchSectionView::deleteCurrentCircuit()
{
    section()->deleteCurrentCircuit();
    patch->commit(tr("deleting circuit ") + currentCircuit()->getName().toUpper());
    emit patchModified();
}

void PatchSectionView::deleteMultipleCircuits(int from, int to)
{
    for (int i=to; i>=from; i--)
        section()->deleteCircuit(i);
    patch->commit(tr("deleting %1 circuits").arg(to - from + 1));
    emit patchModified();
}

void PatchSectionView::deleteCurrentComment()
{
    section()->deleteCurrentComment();
    patch->commit(tr("deleting comment"));
    emit patchModified();
}

void PatchSectionView::deleteCurrentJack()
{
    QString jackName = currentJackAssignment()->jackName();
    section()->deleteCurrentJackAssignment();
    patch->commit(tr("deleting assignment of jack '%1'").arg(jackName));
    emit patchModified();
}

void PatchSectionView::deleteMultipleJacks(int circuitNr, int from, int to)
{
    for (int i=to; i>=from; i--)
        section()->circuit(circuitNr)->deleteJackAssignment(i);
    section()->sanitizeCursor(); // TODO: Das soll die Section selbst machen
    patch->commit(tr("deleting %1 jack assignments").arg(to - from + 1));
    emit patchModified();
}

const Atom *PatchSectionView::currentAtom() const
{
    const JackAssignment *ja = section()->currentJackAssignment();
    if (!ja)
        return 0;
    else {
        int column = section()->cursorPosition().column;
        return ja->atomAt(column);
    }
}

Atom *PatchSectionView::currentAtom()
{
    // TODO: Kann man hier nicht Copy & Paste vermeiden?
    JackAssignment *ja = section()->currentJackAssignment();
    if (!ja)
        return 0;
    else {
        int column = section()->cursorPosition().column;
        return ja->atomAt(column);
    }
}

bool PatchSectionView::atomCellSelected() const
{
    const CursorPosition &cp = section()->cursorPosition();
    return (cp.row >= 0 && cp.column > 0);
}

void PatchSectionView::setCursorPosition(const CursorPosition &pos)
{
    section()->setCursor(pos);
    updateCursor();
}

const CursorPosition &PatchSectionView::getCursorPosition() const
{
    return section()->cursorPosition();
}

void PatchSectionView::deleteCurrentAtom()
{
    JackAssignment *ja = section()->currentJackAssignment();
    int column = section()->cursorPosition().column;
    if (ja->atomAt(column)) {
        QString action = tr("deleting value '%1'").arg(ja->atomAt(column)->toString());
        ja->replaceAtom(column, 0);
        patch->commit(action);
        emit patchModified();
    }
}

void PatchSectionView::deleteMultipleAtoms(int circuitNr, int row, int from, int to)
{
    Circuit *circuit = section()->circuit(circuitNr);
    JackAssignment *ja = circuit->jackAssignment(row);
    bool something = false;
    for (int i=from; i<=to; i++)
        if (ja->atomAt(i))
            something = true;

    if (something) {
        for (int i=from; i<=to; i++)
            ja->replaceAtom(i, 0);
        patch->commit(tr("deleting %1 values of '%2'").arg(to-from+1).arg(ja->jackName()));
        emit patchModified();
    }
}

void PatchSectionView::editCircuit(int key)
{
    if (key)
        return; // Direct editing of circuit currently not possible

    QString oldCircuit = currentCircuitName();
    QString newCircuit = CircuitChooseDialog::chooseCircuit(oldCircuit);
    if (!newCircuit.isEmpty() && oldCircuit != newCircuit)
    {
        currentCircuit()->changeCircuit(newCircuit);
        patch->commit(tr("changing circuit type to '%s'").arg(newCircuit));
        emit patchModified();
    }
}

void PatchSectionView::moveCursorLeftRight(int whence)
{
    if (isEmpty())
        return;

    if (whence == -1)
        section()->moveCursorLeft();
    else
        section()->moveCursorRight();
    emit cursorMoved();
}

void PatchSectionView::moveCursorUpDown(int whence)
{
    if (isEmpty())
        return;

    if (whence == 1) // dowldiln
        section()->moveCursorDown();
    else // up
        section()->moveCursorUp();
    emit cursorMoved();
}
