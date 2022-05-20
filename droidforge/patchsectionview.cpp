#include "patchsectionview.h"
#include "atomoneliner.h"
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

PatchSectionView::PatchSectionView(const Patch *patch, PatchSection *section)
    : patch(patch)
    , section(section)
    , atomSelectorDialog{}
{
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buildPatchSection();
}

PatchSectionView::~PatchSectionView()
{
    deletePatchSection();
    if (atomSelectorDialog)
        delete atomSelectorDialog;
}

void PatchSectionView::buildPatchSection()
{
    QGraphicsScene *scene = new QGraphicsScene();
    QPixmap background(":images/background.png");
    scene->setBackgroundBrush(QBrush(background.scaledToHeight(BACKGROUND_PIXMAP_HEIGHT)));
    setScene(scene);

    int y = 0;
    for (qsizetype i=0; i<section->circuits.size(); i++)
    {
        Circuit *circuit = section->circuits[i];
        CircuitView *cv = new CircuitView(circuit, fontMetrics().lineSpacing());
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
}

bool PatchSectionView::handleKeyPress(int key)
{
    switch (key) {
    case Qt::Key_Up:        moveCursorUpDown(-1);     return true;
    case Qt::Key_Down:      moveCursorUpDown(1);      return true;
    case Qt::Key_Left:      moveCursorLeftRight(-1);  return true;
    case Qt::Key_Right:     moveCursorLeftRight(1);   return true;
    case Qt::Key_PageUp:    moveCursorPageUpDown(-1); return true;
    case Qt::Key_PageDown:  moveCursorPageUpDown(1);  return true;
    case Qt::Key_Backspace: deleteCurrentRow();       return true;
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
    return false;
}


void PatchSectionView::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        if (!handleMousePress(event->pos())) {
            // NIX
        }
    }
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
    ensureVisible(currentCircuitView());
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
        circuit->setComment(newComment);
        the_forge->patchHasChanged();
    }
}

bool PatchSectionView::isEmpty() const
{
    return section->circuits.empty();
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

void PatchSectionView::updateCursor()
{
    currentCircuitView()->select(section->cursorPosition());
    updateRegisterHilites();
}

bool PatchSectionView::handleMousePress(const QPointF &pos)
{
    // itemAt() applies the transformation of the graphics
    // view such as the scroll bar and the alignment.
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());

    if (!item)
        return false;

    CircuitView *cv = (CircuitView *)item;
    QPointF posInScene(mapToScene(pos.toPoint()));
    QPointF posInCircuit = posInScene - item->pos();

    // Loop over all circuits because we need the index number
    // of the clicked circuit, not just the pointer.
    for (unsigned i=0; i<circuitViews.size(); i++)
    {
        if (circuitViews[i] == cv) {
            currentCircuitView()->deselect();
            CursorPosition pos;
            pos.circuitNr = i;
            pos.row = cv->jackAt(posInCircuit.y());
            pos.column = cv->columnAt(posInCircuit.x());
            section->setCursor(pos);
            updateCursor();
            ensureVisible(currentCircuitView());
        }
    }
    return true;
}


CircuitView *PatchSectionView::currentCircuitView()
{
    return circuitViews[section->cursorPosition().circuitNr];
}

const CircuitView *PatchSectionView::currentCircuitView() const
{
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
    ensureVisible(currentCircuitView());
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
    }
    rebuildPatchSection();
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
    ensureVisible(currentCircuitView());
}
