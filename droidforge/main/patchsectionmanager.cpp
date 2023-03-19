#include "patchsectionmanager.h"
#include "clipboard.h"
#include "colorscheme.h"
#include "globals.h"
#include "patchoperator.h"
#include "mainwindow.h"
#include "tuning.h"
#include "updatehub.h"
#include "editoractions.h"
#include "namechoosedialog.h"
#include "patchview.h"
#include "commentdialog.h"
#include "utilities.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QSettings>
#include <QResizeEvent>
#include <QMenu>
#include <QPalette>
#include <QFileDialog>
#include <QMessageBox>

#define DATA_INDEX_SECTION_INDEX 0

PatchSectionManager::PatchSectionManager(MainWindow *mainWindow, PatchEditEngine *patch, QWidget *parent)
    : QGraphicsView{parent}
    , PatchView(patch)
    , mainWindow(mainWindow)
    , lastIndex(-1)
    , dragSectionIndicator(0)
    , dragger(this)
{
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // When you scroll down, the drag indicator will not be shown anymore
    // if the scroll offset > 62.5% of one section height. I don't know why.
    // So for the while I simply shut off the scroll bar.
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setMinimumWidth(PSM_MINIMUM_WIDTH);
    setMaximumWidth(PSM_MAXIMUM_WIDTH);
    setScene(new QGraphicsScene());

    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMouseTracking(true);

    connectActions();
    connectDragger();

    // Events we create
    connect(this, &PatchSectionManager::patchModified, mainWindow->theHub(), &UpdateHub::modifyPatch);
    connect(this, &PatchSectionManager::sectionSwitched, mainWindow->theHub(), &UpdateHub::switchSection);

    // Events we are interested in
    connect(mainWindow->theHub(), &UpdateHub::sectionSwitched, this, &PatchSectionManager::switchSection);
    connect(mainWindow->theHub(), &UpdateHub::patchModified, this, &PatchSectionManager::modifyPatch);

    int width;
    QSettings settings;
    if (settings.contains("patchsectionmanager/size"))
        width = settings.value("patchsectionmanager/size").toInt();
    else
        width = PSM_NORMAL_WIDTH;
    resize(width, height());
}
PatchOperator *PatchSectionManager::theOperator()
{
    return mainWindow->theOperator();
}
void PatchSectionManager::connectActions()
{
    CONNECT_ACTION(ACTION_PREVIOUS_SECTION, &PatchSectionManager::switchBackward);
    CONNECT_ACTION(ACTION_NEXT_SECTION, &PatchSectionManager::switchForward);
    CONNECT_ACTION(ACTION_NEW_PATCH_SECTION, &PatchSectionManager::newSectionAfterCurrent);
    CONNECT_ACTION(ACTION_SAVE_SECTION, &PatchSectionManager::saveSectionAsPatch);
    CONNECT_ACTION(ACTION_PASTE_AS_SECTION, &PatchSectionManager::pasteAsSection);
    CONNECT_ACTION(ACTION_DUPLICATE_PATCH_SECTION, &PatchSectionManager::duplicateSectionDumb);
    CONNECT_ACTION(ACTION_DUPLICATE_PATCH_SECTION_SMARTLY, &PatchSectionManager::duplicateSectionSmartly);
    CONNECT_ACTION(ACTION_DELETE_PATCH_SECTION, &PatchSectionManager::deleteSection);
    CONNECT_ACTION(ACTION_RENAME_PATCH_SECTION, &PatchSectionManager::renameSection);
    CONNECT_ACTION(ACTION_MERGE_WITH_PREVIOUS_SECTION, &PatchSectionManager::mergeWithPreviousSection);
    CONNECT_ACTION(ACTION_MERGE_WITH_NEXT_SECTION, &PatchSectionManager::mergeWithNextSection);
    CONNECT_ACTION(ACTION_MERGE_ALL_SECTIONS, &PatchSectionManager::mergeAllSections);
    CONNECT_ACTION(ACTION_MOVE_SECTION_UP, &PatchSectionManager::moveSectionUp);
    CONNECT_ACTION(ACTION_MOVE_SECTION_DOWN, &PatchSectionManager::moveSectionDown);
    CONNECT_ACTION(ACTION_EDIT_SECTION_COMMENT, &PatchSectionManager::editComment);
}
void PatchSectionManager::connectDragger()
{
    connect(&dragger, &MouseDragger::menuOpenedOnBackground, this, &PatchSectionManager::openMenuOnBackground);
    connect(&dragger, &MouseDragger::menuOpenedOnItem, this, &PatchSectionManager::openMenuOnItem);
    connect(&dragger, &MouseDragger::hoveredIn, this, &PatchSectionManager::hoverIn);
    connect(&dragger, &MouseDragger::hoveredOut, this, &PatchSectionManager::hoverOut);
    connect(&dragger, &MouseDragger::clickedOnItem, this, &PatchSectionManager::clickOnItem);
    connect(&dragger, &MouseDragger::doubleClickedOnBackground, this, &PatchSectionManager::doubleClickOnBackground);
    connect(&dragger, &MouseDragger::doubleClickedOnItem, this, &PatchSectionManager::doubleClickOnItem);
    connect(&dragger, &MouseDragger::itemDragged, this, &PatchSectionManager::dragItem);
    connect(&dragger, &MouseDragger::itemDraggingStopped, this, &PatchSectionManager::stopDraggingItem);
    connect(&dragger, &MouseDragger::draggingAborted, this, &PatchSectionManager::abortDragging);
}
void PatchSectionManager::popupSectionMenu(int index)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_SAVE_SECTION, menu);
    ADD_ACTION(ACTION_PASTE_AS_SECTION, menu);
    if (index >= 0) {
        ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION_SMARTLY, menu);
        ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);

        menu->addSeparator();

        ADD_ACTION_IF_ENABLED(ACTION_MERGE_WITH_PREVIOUS_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_WITH_NEXT_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_ALL_SECTIONS, menu);

        menu->addSeparator();

        ADD_ACTION_IF_ENABLED(ACTION_MOVE_SECTION_UP, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MOVE_SECTION_DOWN, menu);

        menu->addSeparator();

        ADD_ACTION(ACTION_EDIT_SECTION_COMMENT, menu);
        ADD_ACTION(ACTION_EDIT_SECTION_SOURCE, menu);
    }
    menu->popup(QCursor::pos());
}
void PatchSectionManager::resizeEvent(QResizeEvent *event)
{
    if (patch)
        rebuildGraphics();
    QSettings settings;
    settings.setValue("patchsectionmanager/size", width());
    QGraphicsView::resizeEvent(event);
}
void PatchSectionManager::mousePressEvent(QMouseEvent *event)
{
    dragger.mousePress(event);
}
void PatchSectionManager::mouseReleaseEvent(QMouseEvent *event)
{
    dragger.mouseRelease(event);
}
void PatchSectionManager::mouseMoveEvent(QMouseEvent *event)
{
    dragger.mouseMove(event);
}
void PatchSectionManager::mouseDoubleClickEvent(QMouseEvent *event)
{
    dragger.mousePress(event);
}
int PatchSectionManager::clickedSectionIndex(QMouseEvent *event)
{
    for (auto item: items(event->pos())) {
        if (item->data(DATA_INDEX_SECTION_INDEX).isValid()) {
            return  item->data(DATA_INDEX_SECTION_INDEX).toInt();
        }
    }
    return -1;
}
int PatchSectionManager::snapSectionInsertPosition(int fromIndex, float y, float *insertSnap) const
{
    float snapPos = titleViews[0]->pos().y() - PSM_VERTICAL_DISTANCE / 2;
    for (int i=0; i < titleViews.count(); i++)
    {
        if (qAbs(y - snapPos) < PSM_SECTION_SNAP_DISTANCE
                && i != fromIndex && i-1 != fromIndex)
        {
            *insertSnap = snapPos;
            return i;
        }
        snapPos += titleViews[0]->boundingRect().height() + PSM_VERTICAL_DISTANCE;
    }

    // After last section we snap everywhere
    if (fromIndex != titleViews.count() - 1) {
        QRectF r = titleViews.back()->boundingRect().translated(titleViews.back()->pos());
        if (y >= r.bottom()) {
            *insertSnap = r.bottom() + PSM_VERTICAL_DISTANCE / 2;
            return titleViews.count();
        }
    }
    return -1;
}
void PatchSectionManager::modifyPatch()
{
    rebuildGraphics();
}
void PatchSectionManager::switchSection()
{
    updateCursor();
}
void PatchSectionManager::renameSection()
{
    PatchSection *section = patch->currentSection();
    QString oldname = section->getTitle();
    QString newname = NameChooseDialog::getReName(tr("Rename patch section"), tr("New name:"), oldname);
    if (newname != "" && oldname != newname) {
        section->setTitle(newname);
        patch->commit(tr("renaming patch section to '%1'").arg(newname));
        emit patchModified();
    }
}
void PatchSectionManager::deleteSection()
{
    int index = patch->currentSectionIndex();
    QString title = patch->currentSection()->getNonemptyTitle();
    patch->removeSection(index);
    patch->commit(tr("deleting patch section '%1'").arg(title));
    emit patchModified();
}

void PatchSectionManager::duplicateSection(bool smartly)
{
    int index = patch->currentSectionIndex();
    PatchSection *oldSection = patch->section(index);
    QString newname = NameChooseDialog::getNewName(
                smartly ? tr("Duplicate section smartly") : tr("Duplicate section"),
                tr("New name:"),
                oldSection->getTitle());
    if (newname.isEmpty())
        return;

    Patch *newpatch = new Patch();
    PatchSection *newSection = oldSection->clone();
    newSection->setTitle(newname);
    newpatch->addSection(newSection);

    if (smartly) {
        if (!theOperator()->interactivelyRemapRegisters(newpatch)) {
            delete newpatch;
            return;
        }
    }

    PatchSection *newsection = newpatch->section(0)->clone();
    patch->insertSection(index + 1, newsection);
    delete newpatch;
    patch->commit(smartly ? tr("duplicating section smartly") : tr("duplicating section"));
    emit patchModified(); // implies sectionSwitched
}
void PatchSectionManager::duplicateSectionDumb()
{
    duplicateSection(false /* smartly */);
}
void PatchSectionManager::duplicateSectionSmartly()
{
    duplicateSection(true /* smartly */);
}
void PatchSectionManager::mergeWithPreviousSection()
{
    mergeSections(patch->currentSectionIndex(), patch->currentSectionIndex() - 1);
}
void PatchSectionManager::mergeWithNextSection()
{
    mergeSections(patch->currentSectionIndex(), patch->currentSectionIndex() + 1);
}
void PatchSectionManager::mergeSections(int indexa, int indexb)
{
    // Make sure indexa < indexb
    if (indexa > indexb) {
        int x = indexb;
        indexb = indexa;
        indexa = x;
    }
    patch->mergeSections(indexa, indexb);
    patch->commit(tr("merging patch sections"));
    emit patchModified();
}
void PatchSectionManager::newSectionAfterCurrent()
{
    newSectionAtIndex(patch->currentSectionIndex() + 1);
}
void PatchSectionManager::newSectionAtEnd()
{
    newSectionAtIndex(patch->numSections());
}
void PatchSectionManager::saveSectionAsPatch()
{
    const PatchSection *section = patch->currentSection();
    QString newFilePath = QFileDialog::getSaveFileName(
                mainWindow,
                tr("Save section as patch to new file"),
                saveFilename(section->getTitle()),
                tr("DROID patch files (*.ini)"));

    if (newFilePath == "")
        return; // aborted

    QString sectionSource = section->toString(true /* supress empty header */);
    Patch sectionPatch;
    PatchParser parser;
    parser.parseString(sectionSource, &sectionPatch);
    if (section->getTitle() != "")
        sectionPatch.setTitle(section->getTitle());

    if (sectionPatch.saveToFile(newFilePath))
        theOperator()->addToRecentFiles(newFilePath);
    else {
        QMessageBox::warning(
                    mainWindow,
                    tr("Error"),
                    tr("There was an error saving your patch to disk"));
    }
}
void PatchSectionManager::newSectionAtIndex(int index)
{
    QString newname = NameChooseDialog::getNewName(tr("Add new patch section"), tr("Name:"));
    if (newname.isEmpty())
        return;

    patch->insertSection(index, new PatchSection(newname));
    patch->commit(tr("adding new patch section '%1'").arg(newname));
    emit patchModified();
}
void PatchSectionManager::pasteAsSection()
{
    QString newname = NameChooseDialog::getNewName(tr("Paste as new section"), tr("New section name:"));
    if (newname.isEmpty())
        return;
    PatchSection *newSection = new PatchSection(newname);
    for (auto circuit: the_clipboard->getCircuits())
        newSection->addCircuit(circuit->clone());
    int index = patch->currentSectionIndex() + 1;
    patch->insertSection(index, newSection);
    patch->switchCurrentSection(index);
    patch->commit(tr("pasting as new section"));
    emit patchModified();
}
void PatchSectionManager::mergeAllSections()
{
    while (patch->numSections() > 1)
        patch->mergeSections(0, 1);
    patch->commit(tr("merging all patch sections"));
    emit patchModified();
}
void PatchSectionManager::moveSectionUp()
{
    int i = patch->currentSectionIndex();
    patch->moveSection(i, i-1);
    patch->switchCurrentSection(i-1);
    patch->commit(tr("moving section up"));
    emit patchModified();
}
void PatchSectionManager::moveSectionDown()
{
    int i = patch->currentSectionIndex();
    patch->moveSection(i, i+1);
    patch->switchCurrentSection(i+1);
    patch->commit(tr("moving section down"));
    emit patchModified();
}
void PatchSectionManager::editComment()
{
    QString oldComment = section()->getComment().join("\n").trimmed();
    QString newComment = CommentDialog::editComment(tr("Edit section comment"), oldComment);
    if (newComment != oldComment || newComment == "") {
        if (newComment.isEmpty()) {
            QStringList empty;
            section()->setComment(empty);
        }
        else {
            QStringList comment = newComment.trimmed().split('\n');
            section()->setComment(comment);
        }

        patch->commit(tr("editing section comment"));
        emit patchModified();
    }
}
void PatchSectionManager::clickOnItem(QGraphicsItem *item)
{
    switchToSection(item->data(DATA_INDEX_SECTION_INDEX).toInt());
}
void PatchSectionManager::doubleClickOnItem(QGraphicsItem *item)
{
    switchToSection(item->data(DATA_INDEX_SECTION_INDEX).toInt());
    renameSection();
}
void PatchSectionManager::doubleClickOnBackground()
{
    newSectionAtEnd();
}
void PatchSectionManager::openMenuOnBackground()
{
    popupSectionMenu();
}
void PatchSectionManager::openMenuOnItem(QGraphicsItem *item)
{
    int index = item->data(DATA_INDEX_SECTION_INDEX).toInt();
    switchToSection(index);
    popupSectionMenu(index);
}
void PatchSectionManager::hoverIn(QGraphicsItem *)
{
    setCursor(Qt::PointingHandCursor);
}
void PatchSectionManager::hoverOut(QGraphicsItem *)
{
    unsetCursor();
}
void PatchSectionManager::dragItem(QGraphicsItem *startItem, QGraphicsItem *, QPoint pos)
{
    setCursor(Qt::ClosedHandCursor);
    int sectionIndex = startItem->data(DATA_INDEX_SECTION_INDEX).toInt();
    switchToSection(sectionIndex);
    float indicatorPos = pos.y();
    int ip = snapSectionInsertPosition(sectionIndex, pos.y(), &indicatorPos);
    dragSectionIndicator->setSectionRect(startItem->boundingRect().translated(startItem->pos()));
    dragSectionIndicator->setInsertPos(indicatorPos, ip >= 0);
    dragSectionIndicator->setVisible(true);
    dragSectionIndicator->update();

    // Qt seems to have a bug that scene()->update() does not update
    // the whole part of the scene if the visible part is scrolled
    // up too far. We can amend this by artificially faking a larger
    // scene rect while calling update()
    QRectF sceneRect = scene()->sceneRect();
    float h = sceneRect.height() + viewport()->height();
    scene()->setSceneRect(QRectF(0,0, sceneRect.width(), h));
    scene()->update();
    scene()->setSceneRect(sceneRect);
}
void PatchSectionManager::stopDraggingItem(QGraphicsItem *startItem, QGraphicsItem *, QPoint pos)
{
    unsetCursor();
    dragSectionIndicator->setVisible(false);
    int fromIndex = startItem->data(DATA_INDEX_SECTION_INDEX).toInt();
    float indicatorPos = pos.y();
    int toIndex = snapSectionInsertPosition(fromIndex, pos.y(), &indicatorPos);
    if (toIndex >= 0) {
        if (toIndex > fromIndex)
            toIndex--; // index after removing
        patch->moveSection(fromIndex, toIndex);
        patch->switchCurrentSection(toIndex);
        patch->commit(tr("moving section"));
        emit patchModified();
    }
}
void PatchSectionManager::abortDragging()
{
    unsetCursor();
    dragSectionIndicator->setVisible(false);
}
void PatchSectionManager::rebuildGraphics()
{
    scene()->setBackgroundBrush(QBrush(COLOR(COLOR_PATCH_BACKGROUND)));
    if (dragSectionIndicator)
        delete dragSectionIndicator;
    scene()->clear();
    dragSectionIndicator = new DragSectionIndicator();
    dragSectionIndicator->setVisible(false);
    scene()->addItem(dragSectionIndicator);
    titleViews.clear();

    // Add title
    QRectF titleRect(PSM_SIDE_PADDING, PSM_TOP_PADDING,
                            viewport()->width() - 2 * PSM_SIDE_PADDING,
                            CIRV_HEADER_HEIGHT);
    // scene()->addRect(titleRect, QColor(255, 255, 255));
    // QGraphicsTextItem *text = scene()->addText(tr("SECTIONS"));
    // int textWidth = text->boundingRect().width();
    // text->setPos((viewport()->width() - textWidth) / 2, 12);
    // text->setDefaultTextColor(COLOR(PSM_COLOR_TITLE));

    int y = 11; // titleRect.bottom() + 5;
    int width = viewport()->width() - 2 * PSM_SIDE_PADDING;

    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSection *section = patch->section(i);
        unsigned numProblems = patch->numProblemsInSection(i);
        PatchSectionTitleView *item = new PatchSectionTitleView(section->getNonemptyTitle(), width, numProblems);
        titleViews.append(item);
        item->setData(DATA_INDEX_SECTION_INDEX, i);
        item->setData(DATA_INDEX_DRAGGER_PRIO, 1);
        scene()->addItem(item);
        item->setPos(PSM_SIDE_PADDING, y);
        y += item->boundingRect().height() + PSM_VERTICAL_DISTANCE;
    }

    frameCursor = new FrameCursor(mainWindow);
    scene()->addItem(frameCursor);
    QRectF sceneRect(0, 0, viewport()->width(), y);
    scene()->setSceneRect(sceneRect);
    updateCursor();
}
void PatchSectionManager::updateCursor()
{
    int newIndex = patch->currentSectionIndex();
    PatchSectionTitleView *tv = titleViews[newIndex];
    frameCursor->setRect(tv->boundingRect().translated(tv->pos()));
    if (newIndex != lastIndex) {
        frameCursor->startAnimation();
        lastIndex = newIndex;
    }
}
void PatchSectionManager::switchToSection(int i)
{
    if (i != patch->currentSectionIndex()) {
        patch->switchCurrentSection(i);
        emit sectionSwitched();
    }
}
void PatchSectionManager::switchBackward()
{
    switchToSection(qMax(0, patch->currentSectionIndex() - 1));
}
void PatchSectionManager::switchForward()
{
    switchToSection(qMin(patch->numSections() -1 , patch->currentSectionIndex() + 1));
}
