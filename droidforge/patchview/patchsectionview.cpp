#include "patchsectionview.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomoneliner.h"
#include "atomregister.h"
#include "circuitview.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "mainwindow.h"
#include "tuning.h"
#include "commentdialog.h"
#include "circuitchoosedialog.h"

#include <QMouseEvent>
#include <QGraphicsProxyWidget>
#include <QTransform>
#include <QMenu>

PatchSectionView::PatchSectionView(const Patch *patch, PatchSection *section, int zoom)
    : patch(patch)
    , section(section)
    , atomSelectorDialog{}
    , selection(0)
{
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMinimumWidth(CircuitView::minimumWidth() + CIRV_ASSUMED_SCROLLBAR_WIDTH);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    buildPatchSection();
    QPixmap background(":images/background.png");
    setBackgroundBrush(QBrush(background.scaledToHeight(BACKGROUND_PIXMAP_HEIGHT)));
    setZoom(zoom);
}

PatchSectionView::~PatchSectionView()
{
    deletePatchSection();
    if (atomSelectorDialog)
        delete atomSelectorDialog;
}

void PatchSectionView::buildPatchSection()
{
    unsigned circuitWidth = viewport()->width() / zoomFactor;
    QGraphicsScene *scene = new QGraphicsScene();
    setScene(scene);

    int y = 0;
    for (qsizetype i=0; i<section->circuits.size(); i++)
    {
        bool isLast = i == section->circuits.size() - 1;
        Circuit *circuit = section->circuits[i];
        CircuitView *cv = new CircuitView(
                    circuit,
                    i,
                    &selection,
                    circuitWidth,
                    fontMetrics().lineSpacing(),
                    isLast ? CIRV_TOP_PADDING : 0);
        circuitViews.append(cv);
        scene->addItem(cv);
        cv->setPos(0, y); // TODO: der erste parameter wirkt nicht
        y += cv->boundingRect().height();
    }
    if (!isEmpty())
        updateCursor();
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
    // TODO: Scheinbar springt der Viewport hier immer nach oben.
    // Und das ensureVisible bewirkt erstmal nix bis man den Cursor
    // bewegt
}

bool PatchSectionView::handleKeyPress(QKeyEvent *event)
{
    int key = event->key();
    bool shiftHeld = event->modifiers() & Qt::ShiftModifier;

    CursorPosition posBefore = section->cursorPosition();
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
            updateKeyboardSelection(posBefore, section->cursorPosition());
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
    if (event->type() != QMouseEvent::MouseButtonPress)
        return;

    QPoint pos = event->pos();

    // itemAt() applies the transformation of the graphics
    // view such as the scroll bar and the alignment.
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());

    if (item)
    {
        CircuitView *cv = (CircuitView *)item;
        QPointF posInScene(mapToScene(pos));
        QPointF posInCircuit = posInScene - item->pos();

        // Loop over all circuits because we need the index number
        // of the clicked circuit, not just the pointer.
        for (unsigned i=0; i<circuitViews.size(); i++)
        {
            if (circuitViews[i] == cv) {
                CursorPosition curPos;
                curPos.circuitNr = i;
                curPos.row = cv->jackAt(posInCircuit.y());
                curPos.column = cv->columnAt(posInCircuit.x());

                if (event->button() == Qt::LeftButton)
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
    if (event->button() == Qt::RightButton)
        handleRightMousePress(0, CursorPosition());
}

void PatchSectionView::handleLeftMousePress(const CursorPosition &curPos)
{
    currentCircuitView()->deselect();
    if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
        setMouseSelection(curPos);
    else {
        clearSelection();
        section->setCursor(curPos);
    }
    updateCursor();
}

void PatchSectionView::handleRightMousePress(CircuitView *cv, const CursorPosition &curPos)
{
    // TODO: Show a different menu when a selection is active and
    // a cell from the selection is being hit?
    if (selection) {
        qDebug("SELECTIONMENU");
        return;
    }

    // Make sure that cursor is set to the cell the menu is
    // working with. Otherwise all actions would address the
    // wrong cell.
    if (cv) {
        section->setCursor(curPos);
        updateCursor();
    }

    QMenu *menu = new QMenu(this);
    menu->addAction(the_forge->action(ACTION_NEW_CIRCUIT));
    if (cv) {
        menu->addAction(the_forge->action(ACTION_EDIT_VALUE));
        menu->addAction(the_forge->action(ACTION_ADD_JACK));
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
                const Atom *atom = currentAtom();
                if (atom && atom->isCable())
                    menu->addAction(the_forge->action(ACTION_FOLLOW_INTERNAL_CABLE));
                menu->addAction(the_forge->action(ACTION_CREATE_INTERNAL_CABLE));
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

void PatchSectionView::addNewCircuit(QString name, jackselection_t jackSelection)
{
    QString actionTitle = QString("adding new '") + name + "' circuit";
    the_forge->registerEdit(actionTitle);

    int newPosition;
    if (!isEmpty()) {
        currentCircuitView()->deselect();
        newPosition = section->cursorPosition().circuitNr;
        if (section->cursorPosition().row != -2)
            newPosition ++;
    }
    else
        newPosition = 0;

    section->addNewCircuit(newPosition, name, jackSelection);
    rebuildPatchSection();
    updateCursor();
    the_forge->patchHasChanged();
}


void PatchSectionView::addNewJack(QString name)
{
    QString actionTitle = QString("adding new jack '") + name + "' to circuit";
    the_forge->registerEdit(actionTitle);

    int row = section->cursorPosition().row;
    int index = row + 1;
    if (index < 0)
        index = 0;

    currentCircuit()->insertJackAssignment(buildJackAssignment(name), index);
    section->setCursorRow(index);
    section->setCursorColumn(1);
    rebuildPatchSection();
    updateCursor();
    the_forge->patchHasChanged();
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

void PatchSectionView::copyToClipboard(Clipboard &clipboard)
{
    if (selection) {
        clipboard.copyFromSelection(selection, section);
    }
    else {
        Selection sel(section->cursorPosition());
        clipboard.copyFromSelection(&sel, section);
    }
}

Patch *PatchSectionView::getSelectionAsPatch() const
{
    Clipboard cb;
    cb.copyFromSelection(selection, section);
    return cb.getAsPatch();
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
        QString actionTitle = QString("replacing jack with '") + name + "'";
        the_forge->registerEdit(actionTitle);
        // When we convert formerly unknown jacks to known jacks,
        // we need to reparse the still unparsed value expression
        if (ja->jackType() == JACKTYPE_UNKNOWN) {
            JackAssignment *newJa = buildJackAssignment(name);
            newJa->parseExpression(ja->valueToString());
            section->deleteCurrentJackAssignment();
            currentCircuit()->insertJackAssignment(newJa, section->cursorPosition().row);
        }
        else
            currentJackAssignment()->changeJack(name);
        the_forge->patchHasChanged();
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

    int row = section->cursorPosition().row;
    int column = section->cursorPosition().column;

    if (row == -2)
        editCircuit(key);
    else if (row == -1)
        editCircuitComment(key);
    else if (column == 0)
        editJack(key);
    else
        editAtom(key);
}

void PatchSectionView::editAtom(int key)
{
    Circuit *circuit = currentCircuit();
    JackAssignment *ja = circuit->jackAssignment(section->cursorPosition().row);
    if (ja->jackType() == JACKTYPE_UNKNOWN)
        return; // TODO: Edit unknown data anyway?

    const Atom *atom = ja->atomAt(section->cursorPosition().column);
    Atom *newAtom;

    if (key != 0) {
        QPoint posRelativeToScene = currentCircuitView()->frameCursorPosition();
        QPoint posRelativeToView = mapFromScene(posRelativeToScene);
        QPoint posInScreen = mapToGlobal(posRelativeToView);
        QChar c(key);
        QString start(c);
        newAtom = AtomOneliner::editAtom(posInScreen, patch, ja->jackType(), start);
    }
    else
        newAtom = AtomSelectorDialog::editAtom(patch, ja->jackType(), atom);

    if (newAtom != 0 && newAtom != atom) {
        QString actionTitle = QString("changing '") + ja->jackName() + "' to " + newAtom->toString();
        the_forge->registerEdit(actionTitle);
        ja->replaceAtom(section->cursorPosition().column, newAtom);
        updateCursor();
        the_forge->patchHasChanged();
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
        QString actionTitle = QString("changing comment for circuit '") + circuit->getName() + "'";
        the_forge->registerEdit(actionTitle);
        if (newComment != "")
            circuit->setComment(newComment);
        else
            circuit->removeComment();
        rebuildPatchSection();
        the_forge->patchHasChanged();
    }
}

bool PatchSectionView::isEmpty() const
{
    return section->circuits.empty();
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

    CursorPosition cursor = section->cursorPosition();
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
    CursorPosition cursor = section->cursorPosition();
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

    the_forge->registerEdit(tr("inserting register %1").arg(ar.toString()));
    ja->replaceAtom(cursor.column, ar.clone());
    the_forge->patchHasChanged();
}

void PatchSectionView::setZoom(int zoom)
{
    zoomFactor = pow(ZOOM_STEP, double(zoom));
    QTransform transform;
    transform.scale(zoomFactor, zoomFactor);
    setTransform(transform);
    rebuildPatchSection();
}

void PatchSectionView::updateCursor()
{
    if (currentCircuitView()) {
        currentCircuitView()->select(section->cursorPosition());
        QRectF br = currentCircuitView()->boundingRect();
        QRectF tbr = br.translated(currentCircuitView()->pos());
        ensureVisible(tbr);
        updateCableIndicator();
    }
    updateRegisterHilites();
    the_forge->updateActions(); // TODO: Sollte das nicht besser ein Signal sein?
}

void PatchSectionView::updateCableIndicator()
{
    const Atom *atom = currentAtom();
    if (atom && atom->isCable()) {
        AtomCable *ac = (AtomCable *)atom;
        QString name = ac->getCable();
        int numAsOutput = 0;
        int numAsInput = 0;
        patch->findCableConnections(name, numAsInput, numAsOutput);
        the_forge->cableIndicator()->set(name, numAsInput, numAsOutput);
    }
    else
        the_forge->cableIndicator()->clear();
}

void PatchSectionView::setMouseSelection(const CursorPosition &to)
{
    if (selection)
        delete selection;
    selection = new Selection(section->cursorPosition(), to);
    updateCircuits();
    the_forge->updateActions();
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
    the_forge->updateActions();
}

void PatchSectionView::clearSelection()
{
    if (selection) {
        delete selection;
        selection = 0;
        updateCircuits();
        the_forge->updateActions();
    }
}

CircuitView *PatchSectionView::currentCircuitView()
{
    if (circuitViews.isEmpty())
        return 0;
    else
        return circuitViews[section->cursorPosition().circuitNr];
}

const CircuitView *PatchSectionView::currentCircuitView() const
{
    if (circuitViews.isEmpty())
        return 0;
    else
        return circuitViews[section->cursorPosition().circuitNr];
}


Circuit *PatchSectionView::currentCircuit()
{
    return section->currentCircuit();

}

const Circuit *PatchSectionView::currentCircuit() const
{
    return section->currentCircuit();
}


JackAssignment *PatchSectionView::currentJackAssignment()
{
    return section->currentJackAssignment();
}

void PatchSectionView::moveCursorPageUpDown(int whence)
{
    currentCircuitView()->deselect();
    if (whence == -1)
        section->moveCursorToPreviousCircuit();
    else
        section->moveCursorToNextCircuit();
    updateCursor();
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

void PatchSectionView::pasteFromClipboard(Clipboard &clipboard)
{
    if (clipboard.numCircuits()) {
        pasteCircuitsFromClipboard(clipboard);
        qDebug() << "TODO: Hier Dialog wegen anpassen der Register!";
    }
    else if (isEmpty()) {
        // TODO: updateActions in mainwindow soll paste deaktivieren, wenn
        // dieser Fall da ist.
        return;
    }
    else if (clipboard.isComment())
        pasteCommentFromClipboard(clipboard);
    else if (clipboard.numJacks())
        pasteJacksFromClipboard(clipboard);
    else if (clipboard.numAtoms())
        pasteAtomsFromClipboard(clipboard);
    else {
        // Should never happen
    }

}

void PatchSectionView::deleteCurrentRow()
{
    if (isEmpty())
        return;

    const CursorPosition &pos = section->cursorPosition();
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
    QString actionTitle = QString("deleting circuit ") + currentCircuit()->getName().toUpper();
    the_forge->registerEdit(actionTitle);
    section->deleteCurrentCircuit();
    rebuildPatchSection();
    the_forge->patchHasChanged();
}

void PatchSectionView::deleteMultipleCircuits(int from, int to)
{
    QString actionTitle = QString("deleting %1 circuits").arg(to - from + 1);
    the_forge->registerEdit(actionTitle);
    for (int i=to; i>=from; i--)
        section->deleteCircuit(i);
    rebuildPatchSection();
    the_forge->patchHasChanged();
}

void PatchSectionView::deleteCurrentComment()
{
    QString actionTitle = QString("deleting comment");
    the_forge->registerEdit(actionTitle);
    section->deleteCurrentComment();
    rebuildPatchSection();
    updateCursor();
    the_forge->patchHasChanged();
}

void PatchSectionView::deleteCurrentJack()
{
    QString actionTitle = QString("deleting jack ")
            + currentJackAssignment()->jackName() + " assignment";
    the_forge->registerEdit(actionTitle);
    section->deleteCurrentJackAssignment();
    rebuildPatchSection();
    updateCursor();
    the_forge->patchHasChanged();
}

void PatchSectionView::deleteMultipleJacks(int circuitNr, int from, int to)
{
    QString actionTitle = QString("deleting %1 jack assignments").arg(to - from + 1);
    the_forge->registerEdit(actionTitle);
    for (int i=to; i>=from; i--)
        section->circuit(circuitNr)->deleteJackAssignment(i);
    section->sanitizeCursor();
    rebuildPatchSection();
    the_forge->patchHasChanged();
}

const Atom *PatchSectionView::currentAtom() const
{
    JackAssignment *ja = section->currentJackAssignment();
    if (!ja)
        return 0;
    else {
        int column = section->cursorPosition().column;
        return ja->atomAt(column);
    }
}

bool PatchSectionView::atomCellSelected() const
{
    const CursorPosition &cp = section->cursorPosition();
    return (cp.row >= 0 && cp.column > 0);
}

void PatchSectionView::setCursorPosition(const CursorPosition &pos)
{
    currentCircuitView()->deselect();
    section->setCursor(pos);
    updateCursor();
}

void PatchSectionView::deleteCurrentAtom()
{
    JackAssignment *ja = section->currentJackAssignment();
    int column = section->cursorPosition().column;
    if (ja->atomAt(column)) {
        QString actionTitle = QString("deleting value of '") + ja->jackName() + "'";
        the_forge->registerEdit(actionTitle);
        ja->replaceAtom(column, 0);
        the_forge->patchHasChanged();
    }
    rebuildPatchSection();
}

void PatchSectionView::deleteMultipleAtoms(int circuitNr, int row, int from, int to)
{
    Circuit *circuit = section->circuit(circuitNr);
    JackAssignment *ja = circuit->jackAssignment(row);
    bool something = false;
    for (int i=from; i<=to; i++)
        if (ja->atomAt(i))
            something = true;

    if (something) {
        QString actionTitle = QString("deleting %1 values of '%2'").arg(to-from+1).arg(ja->jackName());
        the_forge->registerEdit(actionTitle);
        for (int i=from; i<=to; i++)
            ja->replaceAtom(i, 0);
        the_forge->patchHasChanged();
    }
    rebuildPatchSection();
}

void PatchSectionView::pasteCircuitsFromClipboard(const Clipboard &clipboard)
{
    if (!isEmpty())
        currentCircuitView()->deselect();

    the_forge->registerEdit(tr("pasting %1 circuits").arg(clipboard.getCircuits().count()));
    for (auto circuit: clipboard.getCircuits()) {
        Circuit *newCircuit = circuit->clone();
        int position = section->cursorPosition().circuitNr;
        section->addCircuit(position, newCircuit);
        section->moveCursorToNextCircuit();
    }
    // TODO: Hier klappt das nicht mit der Cursorsichtbarkeit.
    // Das ensureVisible macht irgendwie nix
    the_forge->patchHasChanged();
    rebuildPatchSection();
}

void PatchSectionView::pasteCommentFromClipboard(const Clipboard &clipboard)
{
    QString comment = clipboard.getComment();
    if (comment != currentCircuit()->getComment()) {
        the_forge->registerEdit(tr("pasting circuit comment"));
        currentCircuit()->setComment(comment);
        the_forge->patchHasChanged();
        rebuildPatchSection();
    }
}

void PatchSectionView::pasteJacksFromClipboard(const Clipboard &clipboard)
{
    const QList<JackAssignment *> &jas = clipboard.getJackAssignment();
    the_forge->registerEdit(tr("pasting %1 jack assignments").arg(jas.count()));
    Circuit *circuit = currentCircuit();
    int index = qMin(circuit->numJackAssignments(), qMax(0, section->cursorPosition().row + 1));
    for (auto ja: jas) {
        // We need to reparse the jack assignment, because the circuit we
        // paste into is maybe another circuit and e.g. "clock" might have
        // to change from an input to an output.
        QString asString = ja->toString();
        JackAssignment *jaReparsed = JackAssignment::parseJackLine(circuit->getName(), asString);
        circuit->insertJackAssignment(jaReparsed, index);
        section->setCursorRow(index);
        index++;
    }
    the_forge->patchHasChanged();
    rebuildPatchSection();
}

void PatchSectionView::pasteAtomsFromClipboard(const Clipboard &clipboard)
{
    JackAssignment *ja = currentJackAssignment();
    if (!ja) {
        qDebug() << "das geht hier nicht";
        return;
    }
    int column = section->cursorPosition().column;
    if (column < 1 || column > 3) {
        qDebug() << "das geht hier auch nicht";
        return;
    }
    const QList<Atom *> &atoms = clipboard.getAtoms();
    the_forge->registerEdit(tr("pasting parameters"));
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
        section->setCursorColumn(column);
        column ++;
        if (column > 3)
            break;
    }
    the_forge->patchHasChanged();
    rebuildPatchSection();
}

void PatchSectionView::editCircuit(int key)
{
    if (key)
        return; // Direct editing of circuit currently not possible

    QString oldCircuit = currentCircuitName();
    QString newCircuit = CircuitChooseDialog::chooseCircuit(oldCircuit);
    if (!newCircuit.isEmpty() && oldCircuit != newCircuit)
    {
        QString actionTitle = QString("changing circuit type to '") + newCircuit + "'";
        the_forge->registerEdit(actionTitle);
        currentCircuit()->changeCircuit(newCircuit);
        the_forge->patchHasChanged();
        rebuildPatchSection();
    }
}

void PatchSectionView::moveCursorLeftRight(int whence)
{
    if (isEmpty())
        return;

    if (whence == -1)
        section->moveCursorLeft();
    else
        section->moveCursorRight();

    updateCursor();
}

void PatchSectionView::moveCursorUpDown(int whence)
{
    if (isEmpty())
        return;

    currentCircuitView()->deselect();
    if (whence == 1) // dowldiln
        section->moveCursorDown();
    else // up
        section->moveCursorUp();
    updateCursor();
}
