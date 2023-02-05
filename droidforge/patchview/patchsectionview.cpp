#include "patchsectionview.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomnumber.h"
#include "atomoneliner.h"
#include "atomregister.h"
#include "circuitview.h"
#include "cablecolorizer.h"
#include "colorscheme.h"
#include "globals.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "mainwindow.h"
#include "tuning.h"
#include "commentdialog.h"
#include "circuitchoosedialog.h"
#include "namechoosedialog.h"
#include "circuitchoosedialog.h"
#include "updatehub.h"
#include "jackchoosedialog.h"
#include "patchview.h"
#include "hintdialog.h"

#include <QMouseEvent>
#include <QGraphicsProxyWidget>
#include <QTransform>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QScrollBar>

#define DATA_INDEX_CIRCUIT_NR 0

PatchSectionView::PatchSectionView(PatchEditEngine *initialPatch)
    : PatchView(initialPatch) // patch is never ever 0!
    , zoomLevel(0)
    , zoomFactor(1.0)
    , dragging(false)
{
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    buildPatchSection();
    setBackgroundBrush(COLOR(COLOR_PATCH_BACKGROUND));

    QSettings settings;
    if (settings.contains("patchwindow/zoom"))
        zoomLevel = settings.value("patchwindow/zoom").toInt();
    setZoom(zoomLevel);

    connectActions();

    // Events that we create
    connect(this, &PatchSectionView::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchSectionView::clipboardChanged, the_hub, &UpdateHub::changeClipboard);
    connect(this, &PatchSectionView::selectionChanged, the_hub, &UpdateHub::changeSelection);
    connect(this, &PatchSectionView::sectionSwitched, the_hub, &UpdateHub::switchSection);
    connect(this, &PatchSectionView::cursorMoved, the_hub, &UpdateHub::moveCursor);
    connect(this, &PatchSectionView::patchingChanged, the_hub, &UpdateHub::changePatching);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &PatchSectionView::switchSection);
    connect(the_hub, &UpdateHub::patchModified, this, &PatchSectionView::modifyPatch);
    connect(the_hub, &UpdateHub::selectionChanged, this, &PatchSectionView::changeSelection);
    connect(the_hub, &UpdateHub::cursorMoved, this, &PatchSectionView::moveCursor);
    connect(the_hub, &UpdateHub::patchingChanged, this, &PatchSectionView::changePatching);
    CONNECT_ACTION(ACTION_TEXT_MODE, &PatchSectionView::modifyPatch);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PatchSectionView::clockTick);
    timer->start(10);
}
PatchSectionView::~PatchSectionView()
{
    deletePatchSection();
}
void PatchSectionView::connectActions()
{
    CONNECT_ACTION(ACTION_CUT, &PatchSectionView::cut);
    CONNECT_ACTION(ACTION_COPY, &PatchSectionView::copy);
    CONNECT_ACTION(ACTION_PASTE, &PatchSectionView::paste);
    CONNECT_ACTION(ACTION_PASTE_SMART, &PatchSectionView::pasteSmart);
    CONNECT_ACTION(ACTION_EXPAND_ARRAY, &PatchSectionView::expandArray);
    CONNECT_ACTION(ACTION_EXPAND_ARRAY_MAX, &PatchSectionView::expandArrayMax);
    CONNECT_ACTION(ACTION_ADD_MISSING_JACKS, &PatchSectionView::addMissingJacks);
    CONNECT_ACTION(ACTION_REMOVE_UNDEFINED_JACKS, &PatchSectionView::removeUndefinedJacks);

    CONNECT_ACTION(ACTION_SELECT_ALL, &PatchSectionView::selectAll);
    CONNECT_ACTION(ACTION_SORT_JACKS, &PatchSectionView::sortJacks);
    CONNECT_ACTION(ACTION_DISABLE, &PatchSectionView::disableObjects);
    CONNECT_ACTION(ACTION_ENABLE, &PatchSectionView::enableObjects);
    CONNECT_ACTION(ACTION_NEW_CIRCUIT, &PatchSectionView::newCircuitAtCursor);
    CONNECT_ACTION(ACTION_NEW_JACK, &PatchSectionView::addJack);
    CONNECT_ACTION(ACTION_TOOLBAR_NEW_CIRCUIT, &PatchSectionView::newCircuitAtCursor);
    CONNECT_ACTION(ACTION_TOOLBAR_ADD_JACK, &PatchSectionView::addJack);
    CONNECT_ACTION(ACTION_EDIT_VALUE, &PatchSectionView::editValueByShortcut);
    CONNECT_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, &PatchSectionView::editCircuitComment);
    CONNECT_ACTION(ACTION_EDIT_JACK_COMMENT, &PatchSectionView::editJackComment);
    CONNECT_ACTION(ACTION_EDIT_LABEL, &PatchSectionView::editLabel);
    CONNECT_ACTION(ACTION_RENAME_CABLE, &PatchSectionView::renameCable);
    CONNECT_ACTION(ACTION_RESET_ZOOM, &PatchSectionView::zoomReset);
    CONNECT_ACTION(ACTION_ZOOM_IN, &PatchSectionView::zoomIn);
    CONNECT_ACTION(ACTION_ZOOM_OUT, &PatchSectionView::zoomOut);
    CONNECT_ACTION(ACTION_FOLD_UNFOLD, &PatchSectionView::foldUnfold);
    CONNECT_ACTION(ACTION_FOLD_UNFOLD_ALL, &PatchSectionView::foldUnfoldAll);
    CONNECT_ACTION(ACTION_START_PATCHING, &PatchSectionView::startPatching);
    CONNECT_ACTION(ACTION_FINISH_PATCHING, &PatchSectionView::finishPatching);
    CONNECT_ACTION(ACTION_ABORT_PATCHING, &PatchSectionView::abortPatching);
    CONNECT_ACTION(ACTION_FIND, &PatchSectionView::find);
    CONNECT_ACTION(ACTION_FOLLOW_CABLE, &PatchSectionView::followCable);
    CONNECT_ACTION(ACTION_FOLLOW_REGISTER, &PatchSectionView::followRegister);
}
void PatchSectionView::buildPatchSection()
{
    unsigned totalWidth = viewport()->width() / zoomFactor;
    unsigned circuitWidth = totalWidth - 2 * CIRV_SIDE_PADDING;
    QGraphicsScene *scene = new QGraphicsScene();

    int y = CIRV_TOP_PADDING;
    bool lastWasFolded = false;
    for (qsizetype i=0; i<section()->numCircuits(); i++)
    {
        Circuit *circuit = section()->circuit(i);
        // collapse folded circuits nearer together
        if (circuit->isFolded() && lastWasFolded)
            y = y - CIRV_BOTTOM_PADDING + CIRV_FOLDED_PADDING;
        CircuitView *cv = new CircuitView(
                    circuit,
                    i,
                    section()->getSelectionPointer(),
                    circuitWidth,
                    fontMetrics().lineSpacing());
        cv->setData(DATA_INDEX_CIRCUIT_NR, i);
        circuitViews.append(cv);
        scene->addItem(cv);
        cv->setPos(CIRV_SIDE_PADDING, y);
        lastWasFolded = circuit->isFolded();
        y += cv->boundingRect().height() + CIRV_BOTTOM_PADDING;
    }
    QRectF totalRect(0, 0, totalWidth, y);
    scene->setSceneRect(totalRect);
    frameCursor = new FrameCursor();
    scene->addItem(frameCursor); // owned by the scene now
    updateCursor();
    setScene(scene);
}
void PatchSectionView::createFoldMarkers()
{
    for (unsigned i=0; i<section()->numCircuits(); i++) {
        if (section()->circuit(i)->isFolded()) {
            CursorPosition pos(i, ROW_CIRCUIT, 0);
            placeMarker(pos, ICON_MARKER_FOLDED);
        }
    }
}
void PatchSectionView::createProblemMarkers()
{
    QSet<int> foldedProblemCircuits;

    for (auto problem: patch->allProblems())
    {
        if (problem->getSection() == patch->currentSectionIndex()) {
            const CursorPosition &pos = problem->getCursorPosition();
            Circuit *circuit = section()->circuit(pos.circuitNr);
            if (circuit->isFolded())
                foldedProblemCircuits.insert(pos.circuitNr);
            else
                placeMarker(pos, ICON_MARKER_PROBLEM, problem->getReason());
        }
    }

    // add problem markers for folded circuits
    for (int circuitNr: foldedProblemCircuits) {
        CursorPosition pos(circuitNr, ROW_CIRCUIT, 0);
        placeMarker(pos, ICON_MARKER_PROBLEM, tr("There are problems in this circuit"));
    }
}
void PatchSectionView::createInfoMarkers()
{
    for (unsigned i=0; i<section()->numCircuits(); i++)
    {
        const Circuit *circuit = section()->circuit(i);
        if (circuit->isFolded())
            continue;
        for (unsigned j=0; j<circuit->numJackAssignments(); j++) {
            const JackAssignment *ja = circuit->jackAssignment(j);
            QString comment = ja->getComment();
            if (comment != "")
                placeMarker(CursorPosition(i, j, 0), ICON_MARKER_INFO, comment);
        }
    }
}
void PatchSectionView::createLEDMismatchMarkers()
{
    for (unsigned i=0; i<section()->numCircuits(); i++) {
        if (section()->circuit(i)->hasLEDMismatch()) {
            CursorPosition pos(i, ROW_CIRCUIT, 0);
            placeMarker(pos, ICON_MARKER_LEDMISMATCH,
                        tr("The button and LED definitions of this circuit do not match. Click to fix."));

        }
    }

}
void PatchSectionView::clickOnIconMarker(const IconMarker *marker)
{
    const CursorPosition &pos = marker->cursorPosition();
    section()->setCursor(pos);
    emit cursorMoved();

    switch (marker->getType()) {
    case ICON_MARKER_INFO:
        editJackCommentAt(pos);
        break;

    case ICON_MARKER_FOLDED:
        TRIGGER_ACTION(ACTION_FOLD_UNFOLD);
        break;

    case ICON_MARKER_LEDMISMATCH:
        TRIGGER_ACTION(ACTION_FIX_LED_MISMATCH);
        break;

    default:
        break;
    }
}
void PatchSectionView::editJackCommentAt(const CursorPosition &pos)
{
    JackAssignment *ja = section()->jackAssignmentAt(pos);
    if (!ja)
        return;

    QString oldComment = ja->getComment();
    QString newComment = NameChooseDialog::getName(tr("Edit info of this parameter"), tr("Info:"), oldComment);
    if (newComment != oldComment) {
        ja->setComment(newComment);
        patch->commit(tr("changing info of parameter '%1'").arg(ja->jackName()));
        emit patchModified();
    }
}
void PatchSectionView::editJackComment()
{
    editJackCommentAt(section()->cursorPosition());
}
void PatchSectionView::editLabel()
{
    const Atom *atom = currentAtom();
    if (!atom || !atom->canHaveLabel())
        return; // should be handled by enabling, but better play save

    const AtomRegister *ar = (const AtomRegister *)atom;
    AtomRegister lr = ar->relatedRegisterWithLabel();

    RegisterLabel label = patch->registerLabel(lr);
    QString oldName = label.shorthand;
    QString newName = NameChooseDialog::getName(
                tr("Edit label for register '%1'").arg(atom->toString()),
                tr("Label:"),
                oldName,
                false /* force upper case */);
    if (newName == oldName)
        return;

    label.shorthand = newName;
    patch->setRegisterLabel(lr, label);
    patch->commit(tr("editing register label"));
    emit patchModified();
}
void PatchSectionView::placeMarker(const CursorPosition &pos, icon_marker_t type, const QString &toolTip)
{
    bool text_mode = ACTION(ACTION_TEXT_MODE)->isChecked();

    CircuitView *cv = circuitViews[pos.circuitNr];
    QRectF rect = cv->cellRect(pos.row, pos.column);

    IconMarker *marker = new IconMarker(pos, type, toolTip);
    int x_offset, y_offset = 0;

    if (pos.row == ROW_CIRCUIT) {
        x_offset = cv->nextHeaderMarkerOffset();
        if (text_mode) {
            x_offset -= 4;
            y_offset -= 2;
        }
    }
    else
        x_offset = -rect.height();
    scene()->addItem(marker);

    // If a jack (not an atom) has both a comment and a problem marker
    // they would overlap. We fix this by moving the problem marker to
    // the left if we see that there is a comment. If we once have more
    // than two marker type we can think of something more sophisticated
    // if we like.
    JackAssignment *ja = section()->jackAssignmentAt(pos);
    if (ja && ja->getComment() != "" && pos.column == 0 && type == ICON_MARKER_PROBLEM)
        x_offset -= rect.height();

    if (text_mode) {
        x_offset -= 3;
        y_offset -= 3.5;
    }

    QPointF p(cv->pos().x() + rect.right() + x_offset,
              cv->pos().y() + rect.top() + y_offset);



    marker->setPos(p);
}
void PatchSectionView::deletePatchSection()
{
    scene()->clear();
    circuitViews.clear();
    frameCursor = 0;
}
void PatchSectionView::rebuildPatchSection()
{
    deletePatchSection();
    buildPatchSection();
    if (!ACTION(ACTION_TEXT_MODE)->isChecked())
        createFoldMarkers();
    createInfoMarkers();
    createProblemMarkers();
    createLEDMismatchMarkers();
    updateCursor();
}
bool PatchSectionView::handleKeyPress(QKeyEvent *event)
{
    return handleKeyPress(event->key(), event->modifiers());
}
bool PatchSectionView::handleKeyPress(int key, int modifiers)
{
    shout << "PatchSectionView bekommt KEY" << key << modifiers;
    // If any other modifier than shift is pressed, ignore
    // the movement keys.
    if (modifiers & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))
        return false;

    bool shiftHeld = modifiers & Qt::ShiftModifier;
    CursorPosition posBefore = section()->cursorPosition();
    bool moved = false;

    switch (key) {
    case Qt::Key_Up:        moveCursorUpDown(-1);     moved = true; break;
    case Qt::Key_Down:      moveCursorUpDown(1);      moved = true; break;
    case Qt::Key_Left:      moveCursorLeftRight(-1);  moved = true; break;
    case Qt::Key_Right:     moveCursorLeftRight(1);   moved = true; break;
    case Qt::Key_PageUp:    moveCursorPageUpDown(-1); moved = true; break;
    case Qt::Key_PageDown:  moveCursorPageUpDown(1);  moved = true; break;
    case Qt::Key_Home:      moveCursorHome();         moved = true; break;
    case Qt::Key_End:       moveCursorEnd();          moved = true; break;
    case Qt::Key_Backspace: deleteCursorOrSelection(); return true;
    case Qt::Key_Tab:       moveCursorTab(1);         moved = true; break;
    case Qt::Key_Backtab:   moveCursorTab(-1);        shiftHeld = false; moved = true; break;
    case Qt::Key_Return:
    case Qt::Key_Enter:     if (!the_forge->searchActive())
                                editValue(Qt::Key_Return); break;
    }

    if (moved) {
        if (shiftHeld)
            updateKeyboardSelection(posBefore, section()->cursorPosition());
        else
            if (section()->getSelection())
                the_operator->clearSelection();
        return true;
    }

    // All keys that are used for entering a value popup the atom selector
    // preselected accordingly. Make sure that they are only valid
    // if no modified is pressed.
    const int allowedModifiers = Qt::ShiftModifier | Qt::KeypadModifier;
    if ( ((modifiers | allowedModifiers) == allowedModifiers)
        && ((key >= Qt::Key_A && key <= Qt::Key_Z)
        || key == Qt::Key_Underscore
        || (key >= Qt::Key_0 && key <= Qt::Key_9)
         || key == Qt::Key_Period
         || key == Qt::Key_Minus))
    {
        editValue(key);
        return true;
    }
    return true;
}
void PatchSectionView::mousePressEvent(QMouseEvent *event)
{
    mousePress(event->pos(), event->button(), false);
}
void PatchSectionView::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePress(event->pos(), event->button(), true);
}
CursorPosition *PatchSectionView::cursorAtMousePosition(QPoint pos) const
{
    // itemAt() applies the transformation of the graphics
    // view such as the scroll bar and the alignment.

    CircuitView *cv = 0;
    for (auto item: items(pos)) {
        if (item->data(DATA_INDEX_CIRCUIT_NR).isValid()) {
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
                return new CursorPosition( i,
                                           cv->jackAt(posInCircuit.y()),
                                           cv->columnAt(posInCircuit.x()));
            }
        }
    }
    return 0;
}
void PatchSectionView::mousePress(QPoint pos, int button, bool doubleClick)
{
    // Click on little "info" icon
    if (button == Qt::LeftButton) {
        for (auto item: items(pos)) {
            if (item->data(DATA_INDEX_ICON_MARKER).isValid()) {
                clickOnIconMarker((IconMarker *)item);
                return;
            }
        }
    }

    CursorPosition *curPos = cursorAtMousePosition(pos);
    if (curPos) {
        if (doubleClick && curPos->row == ROW_CIRCUIT)
            foldUnfold();
        else if (doubleClick)
            editValueByMouse(*curPos);
        else if (button == Qt::LeftButton)
            handleLeftMousePress(*curPos);
        else
            handleRightMousePress(curPos);
    }
    else if (button == Qt::RightButton)
        handleRightMousePress(0);
    else {
        the_operator->clearSelection();
        if (doubleClick) {
            int circuitNr = getInsertPosition(mapToScene(pos).y());
            newCircuitAt(circuitNr);
        }
    }
    delete curPos;
}
int PatchSectionView::getInsertPosition(int ypos)
{
    int bestDistance = 99999999;
    int bestCircuit = -1;

    if (ypos >= sceneRect().bottom())
        return section()->numCircuits();

    for (auto item: items()) {
        if (item->data(DATA_INDEX_CIRCUIT_NR).isValid()) {
            CircuitView *cv = (CircuitView *)item;
            int pos = cv->pos().y();
            int dist = abs(ypos - pos);
            if (dist < bestDistance) {
                bestCircuit = item->data(DATA_INDEX_CIRCUIT_NR).toInt();
                bestDistance = dist;
            }
        }
    }
    return bestCircuit;
}
void PatchSectionView::changeSelection()
{
    rebuildPatchSection();
}
void PatchSectionView::modifyPatch()
{
    setBackgroundBrush(COLOR(COLOR_PATCH_BACKGROUND));

    // We are responsible for the unique and complete color decision
    // of all internal cables.
    the_cable_colorizer->colorizeAllCables(patch->allCables());
    if (patch->isPatching())
        abortPatching();
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
void PatchSectionView::changePatching()
{
    rebuildPatchSection(); // remove gray plug if patching was aborted
}
void PatchSectionView::newCircuitAtCursor()
{
    newCircuitAt(-1);
}
void PatchSectionView::newCircuitAt(int position)
{
    jackselection_t jackSelection;
    QString name = CircuitChooseDialog::chooseCircuit(jackSelection);
    if (name != "") {
        section()->addNewCircuit(name, jackSelection, position);
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

    Circuit *circuit = currentCircuit();
    circuit->insertJackAssignment(buildJackAssignment(name), index);
    if (!circuit->isFolded()) {
        section()->setCursorRow(index);
        section()->setCursorColumn(1);
    }
    patch->commit(tr("adding new parameter '%1'").arg(name));
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
    if (the_clipboard->numCircuits())
        pasteCircuitsFromClipboard();
    else if (isEmpty())
        return;
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
    if (!the_operator->interactivelyRemapRegisters(pastedPatch)) {
        delete pastedPatch;
        return;
    }

    int position = 0;
    for (auto circuit: pastedPatch->section(0)->getCircuits()) {
        Circuit *newCircuit = circuit->clone();
        position = section()->isEmpty() ? 0 : section()->cursorPosition().circuitNr + 1;
        section()->insertCircuit(position, newCircuit);
    }
    section()->setCursor(CursorPosition(position, ROW_CIRCUIT, 0));
    patch->commit(tr("smart pasting %1 circuits").arg(the_clipboard->getCircuits().count()));
    emit patchModified();
}
void PatchSectionView::expandArray(bool max)
{
    Circuit *circuit = section()->currentCircuit();
    JackAssignment *ja = section()->currentJackAssignment();
    CursorPosition curPos = section()->cursorPosition();

    // Starting from the *current* index within the jack array find
    // the next hole. Then add the new jack right after the last
    // one before the hole (creating a sane sort order).
    while (true) {
        QString jackName = ja->jackName();
        QString next = circuit->nextJackArrayName(jackName, ja->jackType() == JACKTYPE_INPUT);
        if (next == "")
            break;

        JackAssignment *newJa = ja->clone();
        newJa->setJackName(next);

        // Insert the new jack right after that with the previous index.
        QString prefix = circuit->prefixOfJack(jackName);
        unsigned thisIndex = next.mid(prefix.length()).toUInt();
        if (thisIndex > 1) {
            QString prevName = prefix + QString::number(thisIndex - 1);
            for (unsigned i=0; i<circuit->numJackAssignments(); i++) {
                if (circuit->jackAssignment(i)->jackName() == prevName) {
                    curPos.row = i;
                    break;
                }
            }
        }

        curPos.row++;
        circuit->insertJackAssignment(newJa, curPos.row);
        section()->setCursor(curPos);
        if (!max)
            break;
    }
    patch->commit(tr("expanding parameter array"));
    emit patchModified();
}
void PatchSectionView::expandArrayMax()
{
    expandArray(true);
}
void PatchSectionView::addMissingJacks()
{
    Circuit *circuit = currentCircuit();
    for (auto jackName: circuit->missingJacks(JACKTYPE_INPUT))
        circuit->addJackAssignment(new JackAssignmentInput(jackName));
    for (auto jackName: circuit->missingJacks(JACKTYPE_OUTPUT))
        circuit->addJackAssignment(new JackAssignmentOutput(jackName));
    patch->commit(tr("adding missing parameters"));
    emit patchModified();
}
void PatchSectionView::removeUndefinedJacks()
{
    currentCircuit()->removeUndefinedJacks();
    patch->commit(tr("removing undefined parameters"));
    section()->sanitizeCursor();
    emit patchModified();
}
void PatchSectionView::sortJacks()
{
    const Selection *selection = section()->getSelection();
    if (selection) {
        if (selection->isCircuitSelection()) {
            for (int i=selection->fromPos().circuitNr;
                 i <= selection->toPos().circuitNr;
                 i++)
            {
                section()->circuit(i)->sortJacks();
            }
        }
        else if (!selection->isCommentSelection())
        {
            section()->circuit(selection->fromPos().circuitNr)
                    ->sortJacksFromTo(selection->fromPos().row, selection->toPos().row);
        }
    }
    else if (currentCircuit()) {
        currentCircuit()->sortJacks();
    }
    patch->commit(tr("sorting parameters"));
    emit patchModified();
}
void PatchSectionView::selectAll()
{
    section()->selectAll();
    emit selectionChanged();
}
void PatchSectionView::disableObjects()
{
    enableDisableObjects(false);
}
void PatchSectionView::enableObjects()
{
    enableDisableObjects(true);
}
void PatchSectionView::enableDisableObjects(bool enable)
{
    // Important: There *never* must be an enabled jack on
    // a disable circuit!
    QString what = enable ? tr("enabling") : tr("disabling");
    const Selection *selection = section()->getSelection();
    if (selection) {
        const CursorPosition &fromPos = selection->fromPos();
        const CursorPosition &toPos = selection->toPos();
        if (selection->isCircuitSelection())
        {
            for (int i=fromPos.circuitNr; i<=toPos.circuitNr; i++) {
                Circuit *circuit = section()->circuit(i);
                circuit->setDisabledWithJacks(!enable);
            }
            patch->commit(tr("%1 circuits").arg(what));
            emit patchModified();
        }
        else if (selection->isJackSelection())
        {
            Circuit *circuit = section()->circuit(fromPos.circuitNr);
            if (enable && circuit->isDisabled())
                circuit->setEnabled(); // circuit must be enabled if a jack is enabled
            for (int i=fromPos.row; i<=toPos.row; i++) {
                if (i < 0)
                    continue;
                JackAssignment *ja = circuit->jackAssignment(i);
                ja->setDisabled(!enable);
            }
            patch->commit(tr("%1 parameters").arg(what));
            emit patchModified();
        }
    }
    else {
        JackAssignment *ja = currentJackAssignment();
        if (ja) {
            ja->setDisabled(!enable);
            if (enable && currentCircuit()->isDisabled())
                currentCircuit()->setEnabled();
            patch->commit(tr("%1 parameter").arg(what));
            emit patchModified();
            return;
        }

        Circuit *circuit = currentCircuit();
        if (circuit) {
            circuit->setDisabledWithJacks(!enable);
            patch->commit(tr("%1 circuit").arg(what));
            emit patchModified();
            return;
        }
    }
}
void PatchSectionView::pasteCircuitsFromClipboard()
{
    int insertPosition;
    if (section()->isEmpty())
        insertPosition = 0;
    else if (section()->cursorPosition().row == ROW_CIRCUIT)
        insertPosition = section()->cursorPosition().circuitNr;
    else
        insertPosition = section()->cursorPosition().circuitNr + 1;

    section()->setCursor(CursorPosition(insertPosition,  ROW_CIRCUIT, 0));

    for (auto circuit: the_clipboard->getCircuits()) {
        Circuit *newCircuit = circuit->clone();
        section()->insertCircuit(insertPosition, newCircuit);
        insertPosition++;
    }
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
        // to change from an input to an output. And we need to deal with
        // disabled jack lines correctly here.
        JackAssignment *clone = ja->clone();
        bool jackDisabled = clone->isDisabled();
        clone->setDisabled(false);
        QString asString = clone->toString();
        delete clone;
        JackAssignment *jaReparsed = JackAssignment::parseJackLine(circuit->getName(), asString);
        if (circuit->isDisabled() || jackDisabled)
            jaReparsed->setDisabled(true); // not allowed to paste enabled jacks into disabled circuit

        circuit->insertJackAssignment(jaReparsed, index);
        if (!circuit->isFolded())
            section()->setCursorRow(index);
        index++;
    }
    patch->commit(tr("pasting %1 parameters").arg(jas.count()));
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
                newAtom = JackAssignmentOutput::parseOutputAtom(atom->toString());
            else if (ja->isInput()) {
                if (atom->isNumber()) {
                    const AtomNumber *an = (const AtomNumber *)atom;
                    if (an->isFraction()) {
                        if (column == 2)
                            newAtom = atom->clone();
                        else
                            newAtom = new AtomNumber(an->getNumber(), ATOM_NUMBER_NUMBER);
                    }
                    else
                        newAtom = atom->clone();
                }
                else
                    newAtom = atom->clone();
            }
            else
                newAtom = new AtomInvalid(atom->toDisplay());
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
void PatchSectionView::foldUnfold()
{
    the_operator->clearSelection();
    currentCircuit()->toggleFold();
    section()->setCursorRowColumn(ROW_CIRCUIT, 0);
    patch->commitCursorPosition();
    patch->commitFolding();
    emit patchModified();
}
void PatchSectionView::foldUnfoldAll()
{
    the_operator->clearSelection();
    section()->toggleFold();
    patch->commitCursorPosition();
    patch->commitFolding();
    emit patchModified();
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
    if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
        setMouseSelection(curPos);
    else if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
        instantCopyFrom(curPos);
    else if (QGuiApplication::keyboardModifiers() & Qt::AltModifier)
        instantCableFrom(curPos);

    else {
        the_operator->clearSelection();
        dragging = true;
        section()->setCursor(curPos);
        emit cursorMoved();
    }
}
void PatchSectionView::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
    {
        dragging = false;
        return;
    }

    if (dragging) {
        const CursorPosition *curPos = cursorAtMousePosition(event->pos());
        if (curPos) {
            if (*curPos != section()->cursorPosition())
                setMouseSelection(*curPos);
            delete curPos;
        }
    }
}
void PatchSectionView::handleRightMousePress(const CursorPosition *curPos)
{
    QMenu *menu = new QMenu(this);

    if (section()->getSelection()) {
        ADD_ACTION(ACTION_CUT, menu);
        ADD_ACTION(ACTION_COPY, menu);
        ADD_ACTION(ACTION_PASTE, menu);
        ADD_ACTION(ACTION_PASTE_SMART, menu);
        ADD_ACTION(ACTION_EXPORT_SELECTION, menu);
        ADD_ACTION(ACTION_SORT_JACKS, menu);
    }

    else {
        // Make sure that cursor is set to the cell the menu is
        // working with. Otherwise all actions would address the
        // wrong cell.
        if (curPos) {
            section()->setCursor(*curPos);
            emit cursorMoved();
        }

        ADD_ACTION(ACTION_CUT, menu);
        ADD_ACTION(ACTION_COPY, menu);
        ADD_ACTION(ACTION_PASTE, menu);
        ADD_ACTION(ACTION_PASTE_SMART, menu);
        ADD_ACTION(ACTION_DISABLE, menu);
        ADD_ACTION(ACTION_ENABLE, menu);

        menu->addSeparator();

        ADD_ACTION_IF_ENABLED(ACTION_RENAME_CABLE, menu);
        ADD_ACTION_IF_ENABLED(ACTION_START_PATCHING, menu);
        ADD_ACTION_IF_ENABLED(ACTION_FINISH_PATCHING, menu);
        ADD_ACTION_IF_ENABLED(ACTION_ABORT_PATCHING, menu);
        ADD_ACTION_IF_ENABLED(ACTION_FOLLOW_CABLE, menu);

        menu->addSeparator();

        ADD_ACTION_IF_ENABLED(ACTION_FOLLOW_REGISTER, menu);
        ADD_ACTION(ACTION_CIRCUIT_MANUAL, menu);
        ADD_ACTION(ACTION_SORT_JACKS, menu);
        ADD_ACTION(ACTION_EXPAND_ARRAY, menu);
        ADD_ACTION(ACTION_EXPAND_ARRAY_MAX, menu);
        ADD_ACTION(ACTION_ADD_MISSING_JACKS, menu);
        ADD_ACTION(ACTION_REMOVE_UNDEFINED_JACKS, menu);
        ADD_ACTION_IF_ENABLED(ACTION_FIX_LED_MISMATCH, menu);

        menu->addSeparator();

        ADD_ACTION(ACTION_NEW_CIRCUIT, menu);
        ADD_ACTION(ACTION_NEW_JACK, menu);

        menu->addSeparator();

        ADD_ACTION_IF_ENABLED(ACTION_EDIT_VALUE, menu);
        ADD_ACTION_IF_ENABLED(ACTION_EDIT_CIRCUIT_COMMENT, menu);
        ADD_ACTION_IF_ENABLED(ACTION_EDIT_JACK_COMMENT, menu);
        ADD_ACTION_IF_ENABLED(ACTION_EDIT_LABEL, menu);
        ADD_ACTION_IF_ENABLED(ACTION_EDIT_CIRCUIT_SOURCE, menu);

        menu->addSeparator();

        ADD_ACTION(ACTION_FOLD_UNFOLD, menu);
        ADD_ACTION(ACTION_FOLD_UNFOLD_ALL, menu);
    }

    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->popup(QCursor::pos());
}
void PatchSectionView::resizeEvent(QResizeEvent *)
{
    rebuildPatchSection();
}
JackAssignment *PatchSectionView::buildJackAssignment(const QString &name)
{
    Circuit *circuit = currentCircuit();
    QString circuitName = circuit->getName();
    if (the_firmware->jackIsInput(circuitName, name)) {
        auto ja = new JackAssignmentInput(name);
        if (the_firmware->jackHasDefaultvalue(circuitName, name)) {
            double default_value = the_firmware->jackDefaultvalue(circuitName, name);
            AtomNumber *an = new AtomNumber(default_value, ATOM_NUMBER_NUMBER);
            ja->replaceAtom(1, an);
        }
        return ja;
    }
    else if (the_firmware->jackIsOutput(circuitName, name))
        return new JackAssignmentOutput(name);
    else
        return new JackAssignmentUnknown(name);
}
void PatchSectionView::copyToClipboard()
{
    const Selection *selection = section()->getSelection();
    if (selection) {
        the_clipboard->copyFromSelection(selection, section());
    }
    else {
        Selection sel(section()->cursorPosition());
        the_clipboard->copyFromSelection(&sel, section());
    }
    the_clipboard->copyToGlobalClipboard();
    emit clipboardChanged();
}
void PatchSectionView::startPatching()
{
    patch->startPatching();
    emit patchingChanged();
    HintDialog::hint("start_patching",
      tr("You are going to create an internal patch connection from the current\n"
         "parameter to some other parameter of another circuit. Move the cursor\n"
         "to the target location and press Enter to create a connection.\n\n"
         "Make sure that every cable is connected to exactly one output parameter\n"
         "and to one or more input parameters."));
}
void PatchSectionView::finishPatching()
{
    Q_ASSERT(patch->isPatching());

    PatchSection *startSection = patch->getPatchingStartSection();
    CursorPosition startPos = patch->getPatchingStartPosition();
    const Atom *startAtom = startSection->atomAt(startPos);

    PatchSection *endSection = patch->currentSection();
    CursorPosition endPos = endSection->cursorPosition();
    const Atom *endAtom = endSection->atomAt(endPos);

    QString cableName;
    if (startAtom && startAtom->isCable())
        cableName = ((AtomCable *)startAtom)->getCable();
    else if (endAtom && endAtom->isCable())
        cableName = ((AtomCable *)endAtom)->getCable();
    else {
        cableName = NameChooseDialog::getName(tr("Create new internal patch cable"), tr("Cable name:"), "", true);
        if (cableName == "") {
            return;
        }
        cableName = cableName.toUpper();
    }

    startSection->setAtomAt(startPos, new AtomCable(cableName));
    endSection->setAtomAt(endPos, new AtomCable(cableName));
    patch->commit(tr("creating internal cable '%1'").arg(cableName));
    emit patchingChanged();
    emit patchModified();
}
void PatchSectionView::abortPatching()
{
    patch->stopPatching();
    emit patchingChanged();
}
void PatchSectionView::followCable()
{
    const Atom *currentAtom = patch->currentAtom();
    Q_ASSERT(currentAtom && currentAtom->isCable()); // sonst wär die Aktion disabled

    QString name = ((AtomCable *)currentAtom)->getCable();

    bool waitForNext = false;
    bool found = false;

    // First try to find the next atom *after* the current
    Patch::iterator it = patch->begin();
    while (*it) {
        Atom *atom = *it;
        if (atom == currentAtom)
            waitForNext = true;
        else if (waitForNext && atom->isCable() && ((AtomCable *)atom)->getCable() == name) {
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
            if (atom->isCable() && ((AtomCable *)atom)->getCable() == name) {
                found = true;
                break;
            }
            ++it;
        }
    }

    if (!found)
        return;

    the_operator->jumpTo(it.sectionIndex(), it.cursorPosition());
}
void PatchSectionView::followRegister()
{
    const Atom *currentAtom = patch->currentAtom();
    Q_ASSERT(currentAtom && currentAtom->isRegister());
    const AtomRegister *areg = (const AtomRegister *)currentAtom;

    bool waitForNext = false;
    bool found = false;

    // First try to find the next atom *after* the current
    Patch::iterator it = patch->begin();
    while (*it) {
        const Atom *atom = *it;
        if (atom == currentAtom)
            waitForNext = true;
        else if (waitForNext
                 && atom->isRegister()
                 && ((const AtomRegister *)atom)->isRelatedTo(*areg)) {
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
            if (atom->isRegister()
                 && ((const AtomRegister *)atom)->isRelatedTo(*areg)) {
                found = true;
                break;
            }
            ++it;
        }
    }

    if (!found)
        return;

    the_operator->jumpTo(it.sectionIndex(), it.cursorPosition());

}
void PatchSectionView::find()
{
    shout << "FIND";
}
void PatchSectionView::editJack(int key)
{
    if (key != Qt::Key_Return)
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
        patch->commit(tr("replacing parameter with '%1'").arg(name));
        emit patchModified();
    }
}
QString PatchSectionView::currentCircuitName() const
{
    // We assume that there is a current circuit!
    return currentCircuit()->getName();
}
QStringList PatchSectionView::usedJacks() const
{
    return currentCircuitView()->usedJacks();
}
void PatchSectionView::editValueByShortcut()
{
    shout << "Das hier kann weg";
    editValue(Qt::Key_Return);
}
void PatchSectionView::editValue(int key)
{
    if (isEmpty())
        return;

    int row = section()->cursorPosition().row;
    int column = section()->cursorPosition().column;

    if (row == ROW_CIRCUIT)
        editCircuit(key);
    else if (row == ROW_COMMENT)
        editCircuitComment(key);
    else if (column == 0 && key == 0)
        editJackComment();
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
    int lastKey = 0;

    if ((key == 0 || key == Qt::Key_Return) && patch->isPatching()) {
        finishPatching();
        return;
    }

    Circuit *circuit = currentCircuit();
    JackAssignment *ja = circuit->jackAssignment(section()->cursorPosition().row);

    CursorPosition curPos = section()->cursorPosition();
    const Atom *atom = ja->atomAt(curPos.column);
    Atom *newAtom;

    if ((key != 0 && key != Qt::Key_Return) || ja->jackType() == JACKTYPE_UNKNOWN) {
        QRectF cursor = currentCircuitView()->cellRect(curPos.row, curPos.column).translated(currentCircuitView()->pos());
        QPointF topleftRelativeToScene = cursor.topLeft();
        QPointF botrightRelativeToScene = cursor.bottomRight();
        QPoint topleftRelativeToView = mapFromScene(topleftRelativeToScene);
        QPoint botrightRelativeToView = mapFromScene(botrightRelativeToScene);
        QPoint topleftInScreen = mapToGlobal(topleftRelativeToView);
        QPoint botrightInScreen = mapToGlobal(botrightRelativeToView);
        QRectF geometry(topleftInScreen, botrightInScreen);

        QChar c((key > 0 && key < 127) ? key : 0);
        QString start(c);
        newAtom = AtomOneliner::editAtom(geometry, patch, ja->jackType(), start, lastKey);
        if (newAtom && newAtom->isNumber() && ((AtomNumber *)newAtom)->isFraction()
                && curPos.column != 2)
        {
            Atom *n = new AtomNumber(((AtomNumber *)newAtom)->getNumber(), ATOM_NUMBER_NUMBER);
            delete newAtom;
            newAtom = n;
        }
    }
    else {
        newAtom = AtomSelectorDialog::editAtom(patch, circuit->getName(), ja->jackName(), ja->jackType(), curPos.column == 2, atom);
    }

    if (newAtom != 0 && newAtom != atom) {
        ja->replaceAtom(section()->cursorPosition().column, newAtom);
        patch->commit(tr("changing parameter '%1'").arg(ja->jackName()));
        if (lastKey == Qt::Key_Return) {
            if (section()->cursorPosition().row + 1 < currentCircuit()->numJackAssignments())
                section()->moveCursorDown();
            lastKey = 0;
        }
        emit patchModified();
    }
    if (lastKey) {
        handleKeyPress(lastKey, 0);
    }
}
void PatchSectionView::editCircuitComment(int key)
{
    Circuit *circuit = currentCircuit();

    QString oldComment;
    if (key && key != Qt::Key_Return)
        oldComment = QString(QChar(key));
    else
        oldComment = circuit->getComment();
    QString newComment = CommentDialog::editComment(oldComment);
    if (newComment != oldComment) {
        if (newComment != "")
            circuit->setComment(newComment);
        else
            circuit->removeComment();
        if (!circuit->isFolded())
            section()->setCursorRow(ROW_COMMENT);
        patch->commit(tr("changing comment for circuit '%1'").arg(circuit->getName()));
        emit patchModified();
    }
}
void PatchSectionView::renameCable()
{
    const Atom *atom = currentAtom();
    if (!atom || !atom->isCable())
        return;

    QString oldName = ((AtomCable *)atom)->getCable();
    QString newName = NameChooseDialog::getName(
                tr("Rename internal cable '%1'").arg(oldName),
                tr("New name:"),
                oldName,
                true /* force upper case */);
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
    return section()->isEmpty();
}
void PatchSectionView::updateCircuits()
{
    for (unsigned i=0; i<circuitViews.size(); i++)
        circuitViews[i]->update();
}
void PatchSectionView::clickOnRegister(AtomRegister ar)
{
    CursorPosition cursor = section()->cursorPosition();
    if (cursor.row < 0) return;
    JackAssignment *ja = currentCircuit()->jackAssignment(cursor.row);

    int column = qMax(1, cursor.column); // allow cursor on jack name

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

    // The following is a conveniance feature for that case that
    // you want to use LEDs as inputs (as often done with buttons
    // or buttongroups). You click on the button and get e.g. B1.1.
    // Now if you click again, that changes to L1.1 (and later back again)
    else if (ja->isInput()) {
        const Atom *a = ja->atomAt(column);
        if (a && a->isRegister()) {
            const AtomRegister *arx = (AtomRegister *)a;
            if (*arx == ar && ar.getRegisterType() == REGISTER_BUTTON) {
                HintDialog::hint("click_button_led",
                  tr("You just clicked on button %1 to assign that button\n"
                     "to the input parameter \"%2\" in your patch. But that\n"
                     "button was already assigned or you clicked two times in a row.\n\n"
                     "Clicking a button a second time you changes the reference to\n"
                     "the button into the LED in that button. Using an LED as input\n"
                     "is allowed and can sometimes be useful.").arg(ar.toString()).arg(ja->jackName()));
                ar.setRegisterType(REGISTER_LED);
            }
        }
    }
    ja->replaceAtom(column, ar.clone());

    const Circuit *circuit = currentCircuit();
    if (circuit->numJackAssignments() > cursor.row + 1
        && ja->jackPrefix() == circuit->jackAssignment(cursor.row + 1)->jackPrefix())
    {
        cursor.row++;
        section()->setCursor(cursor);
    }
    patch->commit(tr("inserting register %1").arg(ar.toString()));
    emit patchModified();
}
void PatchSectionView::clockTick()
{
    if (needScrollbarAdaption) {
        QRectF c = frameCursor->boundingRect();
        QRectF cursorRect(c.left(), c.top() - CURSOR_VISIBILITY_MARGIN,
                          c.width(), c.height() + 2 * CURSOR_VISIBILITY_MARGIN);
        QRectF portRect = viewport()->contentsRect();
        QRectF sceneRect = mapToScene(portRect.toRect()).boundingRect();

        int visibleTop = sceneRect.top();
        int visibleBottom = sceneRect.bottom();
        int cursorTop = c.top() - CURSOR_VISIBILITY_MARGIN;
        int cursorBottom = c.bottom() + CURSOR_VISIBILITY_MARGIN;

        int dir;

        // The top of the cursor is above the visible area?
        // The lets move up.
        if (cursorTop < visibleTop)
            dir = cursorTop - visibleTop;
        else if (cursorBottom > visibleBottom && cursorTop - visibleTop > CURSOR_VISIBILITY_MARGIN)
            dir = cursorBottom - visibleBottom;
        else {
            dir = 0;
            needScrollbarAdaption = false;
        }

        int pos = verticalScrollBar()->value();
        if (pos == 0 && dir < 0)
            needScrollbarAdaption = false;
        else if (pos == verticalScrollBar()->maximum() && dir > 0)
            needScrollbarAdaption = false;
        else if (dir) {
            float moveBy = dir * zoomFactor * PSV_SCROLL_SPEED;
            if (moveBy > 0 && moveBy < 1)
                moveBy = 1;
            else if (moveBy < 0 && moveBy > -1)
                moveBy = -1;

            pos += moveBy;
            verticalScrollBar()->setValue(pos);
            scene()->update();
        }
    }
}
void PatchSectionView::updateCursor()
{
    if (currentCircuit()) {
        const CursorPosition &pos = section()->cursorPosition();

        if (currentCircuit()->isDisabled() ||
                (currentJackAssignment() && currentJackAssignment()->isDisabled()))
            frameCursor->setMode(CURSOR_DISABLED);
        else if (patch->isPatching())
            frameCursor->setMode(CURSOR_PATCHING);
        else if (patch->problemAt(patch->currentSectionIndex(), pos) != "")
            frameCursor->setMode(CURSOR_PROBLEM);
        else
            frameCursor->setMode(CURSOR_NORMAL);

        QRectF cellRect = currentCircuitView()->cellRect(pos.row, pos.column);
        frameCursor->setRect(cellRect.translated(currentCircuitView()->pos()));
        frameCursor->startAnimation();
        frameCursor->setVisible(true);
        needScrollbarAdaption = true;
    }
    else
        frameCursor->setVisible(false);
}
void PatchSectionView::clearSettings()
{
    setZoom(0);
}
void PatchSectionView::setMouseSelection(const CursorPosition &to)
{
    section()->setMouseSelection(to);
    emit selectionChanged();
}
void PatchSectionView::instantCopyFrom(const CursorPosition &from)
{
    if (from.row < 0 || from.column <= 0) // not on an atom
        return;

    else if (from == section()->cursorPosition()) // on same cell
        return;

    const Atom *sourceAtom = section()->atomAt(from);
    if (!sourceAtom)
        return; // source is empty

    JackAssignment *ja = currentJackAssignment();
    ja->replaceAtom(section()->cursorPosition().column, sourceAtom->clone());
    patch->commit(tr("copying '%1'").arg(sourceAtom->toString()));
    emit patchModified();
}
void PatchSectionView::instantCableFrom(const CursorPosition &from)
{
    if (from.row < 0 || from.column <= 0) // not on an atom
        return;

    const Atom *atom = currentAtom();
    const Atom *sourceAtom = section()->atomAt(from);

    QString cableName;
    if (sourceAtom && sourceAtom->isCable())
        cableName = ((AtomCable *)sourceAtom)->getCable();
    else if (atom && atom->isCable())
        cableName = ((AtomCable *)atom)->getCable();
    else
        cableName = patch->freshCableName();

    AtomCable cable(cableName);

    auto pos = section()->cursorPosition();

    // Add the cable both to from and to me. If from is empty,
    // the direction will be reversed.
    currentJackAssignment()->replaceAtom(pos.column, cable.clone());
    JackAssignment *ja = section()->jackAssignmentAt(from);
    ja->replaceAtom(from.column, cable.clone());
    patch->commit(tr("creating cable '%1'").arg(cableName));
    emit patchModified();

}
void PatchSectionView::updateKeyboardSelection(const CursorPosition &before, const CursorPosition &after)
{
    if (!section()->isEmpty())
    {
        section()->updateKeyboardSelection(before, after);
        emit selectionChanged();
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
void PatchSectionView::moveCursorHome()
{
    if (!section()->isEmpty())
        section()->setCursor(CursorPosition(0, ROW_CIRCUIT, 0));
    emit cursorMoved();
}
void PatchSectionView::moveCursorEnd()
{
    if (!section()->isEmpty())
        section()->setCursor(CursorPosition(section()->numCircuits()-1, ROW_CIRCUIT, 0));
    emit cursorMoved();
}
void PatchSectionView::deleteCursorOrSelection()
{
    if (section()->getSelection()) {
        Selection selection = *section()->getSelection();
        the_operator->clearSelection();

        // When just a single object is selected, we can use
        // the existing functions for deleting at the cursor position
        if (selection.isCommentSelection())
            deleteCurrentComment(); // cursor must be in selection
        else if (selection.isSingleCircuitSelection())
            deleteCurrentCircuit();
        else if (selection.isSingleJackSelection())
            deleteCurrentJack();
        else if (selection.isSingleAtomSelection())
            deleteCurrentAtom();

        // Multiple selection
        else if (selection.isCircuitSelection())
            deleteMultipleCircuits(selection.fromPos().circuitNr,
                           selection.toPos().circuitNr);
        else if (selection.isJackSelection())
            deleteMultipleJacks(selection.fromPos().circuitNr,
                                selection.fromPos().row,
                                selection.toPos().row);
        else if (selection.isAtomSelection())
            deleteMultipleAtoms(selection.fromPos().circuitNr,
                                selection.fromPos().row,
                                selection.fromPos().column,
                                selection.toPos().column);
    }
    else {
        deleteCurrentRow();
    }
}
void PatchSectionView::deleteCurrentRow()
{
    if (isEmpty())
        return;

    const CursorPosition &pos = section()->cursorPosition();
    if (pos.row == ROW_CIRCUIT)
        deleteCurrentCircuit();
    else if (pos.row == ROW_COMMENT)
        deleteCurrentComment();
    else if (pos.column == 0)
        deleteCurrentJack();
    else
        deleteCurrentAtom();
}
void PatchSectionView::deleteCurrentCircuit()
{
    QString name = currentCircuit()->getName().toUpper();
    section()->deleteCurrentCircuit();
    patch->commit(tr("deleting circuit '%1'").arg(name));
    emit patchModified();
}
void PatchSectionView::deleteMultipleCircuits(int from, int to)
{
    section()->setCursor(CursorPosition(from, ROW_CIRCUIT, 0));
    int count = to - from + 1;
    while (count--)
        section()->deleteCircuit(from);
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
    patch->commit(tr("deleting parameter '%1'").arg(jackName));
    emit patchModified();
}
void PatchSectionView::deleteMultipleJacks(int circuitNr, int from, int to)
{
    for (int i=to; i>=from; i--)
        section()->circuit(circuitNr)->deleteJackAssignment(i);
    section()->sanitizeCursor();
    patch->commit(tr("deleting %1 parameters").arg(to - from + 1));
    emit patchModified();
}
const Atom *PatchSectionView::currentAtom() const
{
    return (const_cast<PatchSectionView *>(this))->currentAtom();
}
Atom *PatchSectionView::currentAtom()
{
    JackAssignment *ja = section()->currentJackAssignment();
    if (!ja)
        return 0;
    else {
        int column = section()->cursorPosition().column;
        if (!ja->isInput())
            column = qMin(1, column);
        return ja->atomAt(column);
    }
}
bool PatchSectionView::atomCellSelected() const
{
    const CursorPosition &cp = section()->cursorPosition();
    return (cp.row >= 0 && cp.column > 0);
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
        patch->commit(tr("changing circuit type to '%1'").arg(newCircuit));
        emit patchModified();
    }
}
void PatchSectionView::moveCursorLeftRight(int whence)
{
    if (isEmpty())
        return;

    CursorPosition pos = section()->cursorPosition();

    if (whence == -1) {
        if (pos.row == ROW_CIRCUIT && !currentCircuit()->isFolded()) {
            foldUnfold();
            return;
        }
        section()->moveCursorLeft();
    }
    else {
        if (pos.row == ROW_CIRCUIT && currentCircuit()->isFolded()) {
            foldUnfold();
            return;
        }
        section()->moveCursorRight();
    }
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
void PatchSectionView::moveCursorTab(int whence)
{
    if (isEmpty())
        return;

    if (whence == 1)
        section()->moveCursorForward();
    else
        section()->moveCursorBackward();
    emit cursorMoved();
}
