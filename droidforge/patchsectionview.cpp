#include "patchsectionview.h"
#include "circuitview.h"
#include "mainwindow.h"
#include "tuning.h"

#include <QMouseEvent>

PatchSectionView::PatchSectionView(PatchSection *section)
    : section(section)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buildPatchSection();
}


void PatchSectionView::buildPatchSection()
{
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(COLOR_PATCH_BACKGROUND);
    setScene(scene);

    unsigned y = CIRCUIT_VERTICAL_MARGIN;
    for (qsizetype i=0; i<section->circuits.size(); i++)
    {
        Circuit *circuit = section->circuits[i];
        CircuitView *cv = new CircuitView(circuit);
        circuitViews.append(cv);
        scene->addItem(cv);
        cv->setPos(0, y); // TODO: der erste parameter wirkt nicht
        y += cv->boundingRect().height();
    }
    currentCircuitView()->select(section->cursorPosition());
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
    case Qt::Key_Up:       moveCursorUpDown(-1);     return true;
    case Qt::Key_Down:     moveCursorUpDown(1);      return true;
    case Qt::Key_Left:     moveCursorLeftRight(-1);  return true;
    case Qt::Key_Right:    moveCursorLeftRight(1);   return true;
    case Qt::Key_PageUp:   moveCursorPageUpDown(-1); return true;
    case Qt::Key_PageDown: moveCursorPageUpDown(1);  return true;
    case Qt::Key_Backspace: deleteCurrentRow();      return true;
    default: return false;
    }
}


void PatchSectionView::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        if (!handleMousePress(event->pos())) {
            qDebug() << "Unhandled mouse press" << event;
        }
    }
}


bool PatchSectionView::handleMousePress(const QPointF &pos)
{
    // itemAt() applies the transformation of the graphics
    // view such as the scroll bar and the alignment.
    QGraphicsItem *item = this->itemAt(pos.x(), pos.y());

    if (!item)
        return false;

    CircuitView *cv = (CircuitView *)item;
    for (unsigned i=0; i<circuitViews.size(); i++)
    {
        // Now we need the coordinates relative to the circuitview
        // itself.
        QPointF posInScene(mapToScene(pos.toPoint()));
        QPointF posInCircuit = posInScene - item->pos();

        if (circuitViews[i] == cv) {
            currentCircuitView()->deselect();
            CursorPosition pos;
            pos.circuitNr = i;
            pos.row = cv->jackAt(posInCircuit.y());
            pos.column = cv->columnAt(posInCircuit.x());
            currentCircuitView()->select(pos);
            section->setCursor(pos);
            ensureVisible(currentCircuitView());
        }
    }
    return true;
}


CircuitView *PatchSectionView::currentCircuitView()
{
    return circuitViews[section->cursorPosition().circuitNr];
}


Circuit *PatchSectionView::currentCircuit()
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
    currentCircuitView()->select(section->cursorPosition());
    ensureVisible(currentCircuitView());
}


void PatchSectionView::deleteCurrentRow()
{
    if (section->cursorPosition().row == -1)
        deleteCurrentCircuit();
    else
        deleteCurrentJack();
}


void PatchSectionView::deleteCurrentCircuit()
{
    QString actionTitle = QString("Deleting circuit ") + currentCircuit()->getName().toUpper();
    the_forge->registerEdit(actionTitle);
    section->deleteCurrentCircuit();
    rebuildPatchSection();
}


void PatchSectionView::deleteCurrentJack()
{
    QString actionTitle = QString("Deleting jack ")
            + currentJackAssignment()->jackName() + " assignment";
    the_forge->registerEdit(actionTitle);
    section->deleteCurrentJackAssignment();
    rebuildPatchSection();
    currentCircuitView()->select(section->cursorPosition());
}


void PatchSectionView::moveCursorLeftRight(int whence)
{
    if (whence == -1)
        section->moveCursorLeft();
    else
        section->moveCursorRight();
    currentCircuitView()->select(section->cursorPosition());
}


void PatchSectionView::moveCursorUpDown(int whence)
{
    currentCircuitView()->deselect();

    if (whence == 1) // dowldiln
        section->moveCursorDown();
    else // up
        section->moveCursorUp();

    currentCircuitView()->select(section->cursorPosition());
    ensureVisible(currentCircuitView());
}
