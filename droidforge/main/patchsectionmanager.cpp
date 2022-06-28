#include "patchsectionmanager.h"
#include "clipboard.h"
#include "colorscheme.h"
#include "globals.h"
#include "patchoperator.h"
#include "tuning.h"
#include "updatehub.h"
#include "editoractions.h"
#include "namechoosedialog.h"
#include "patchview.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QSettings>
#include <QResizeEvent>
#include <QMenu>
#include <QPalette>

#define DATA_INDEX_SECTION_INDEX 0

PatchSectionManager::PatchSectionManager(PatchEditEngine *patch, QWidget *parent)
    : QGraphicsView{parent}
    , PatchView(patch)
    , lastIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumWidth(150);
    setMaximumWidth(400);
    setScene(new QGraphicsScene());

    scene()->setBackgroundBrush(QBrush(the_colorscheme->backgroundPixmap()));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMouseTracking(true);

    // Actions we handle
    connectActions();

    // Events we create
    connect(this, &PatchSectionManager::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchSectionManager::sectionSwitched, the_hub, &UpdateHub::switchSection);

    // Events we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &PatchSectionManager::switchSection);
    connect(the_hub, &UpdateHub::patchModified, this, &PatchSectionManager::modifyPatch);

    QSettings settings;
    int width = settings.value("patchsectionmanager/size").toInt();
    resize(width, height());
}

void PatchSectionManager::connectActions()
{
    CONNECT_ACTION(ACTION_PREVIOUS_SECTION, &PatchSectionManager::switchBackward);
    CONNECT_ACTION(ACTION_NEXT_SECTION, &PatchSectionManager::switchForward);
    CONNECT_ACTION(ACTION_NEW_PATCH_SECTION, &PatchSectionManager::newSectionAfterCurrent);
    CONNECT_ACTION(ACTION_PASTE_AS_SECTION, &PatchSectionManager::pasteAsSection);
    CONNECT_ACTION(ACTION_DUPLICATE_PATCH_SECTION, &PatchSectionManager::duplicateSection);
    CONNECT_ACTION(ACTION_DELETE_PATCH_SECTION, &PatchSectionManager::deleteSection);
    CONNECT_ACTION(ACTION_RENAME_PATCH_SECTION, &PatchSectionManager::renameSection);
    CONNECT_ACTION(ACTION_MERGE_WITH_PREVIOUS_SECTION, &PatchSectionManager::mergeWithPreviousSection);
    CONNECT_ACTION(ACTION_MERGE_WITH_NEXT_SECTION, &PatchSectionManager::mergeWithNextSection);
    CONNECT_ACTION(ACTION_MERGE_ALL_SECTIONS, &PatchSectionManager::mergeAllSections);
}

void PatchSectionManager::popupSectionMenu(int index)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_PASTE_AS_SECTION, menu);
    if (index >= 0) {
        ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_WITH_PREVIOUS_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_WITH_NEXT_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_ALL_SECTIONS, menu);
        menu->addSeparator();
        ADD_ACTION(ACTION_EDIT_SECTION_SOURCE, menu);
    }
    menu->popup(QCursor::pos());
}

void PatchSectionManager::resizeEvent(QResizeEvent *)
{
    if (patch)
        rebuildGraphics();
    QSettings settings;
    settings.setValue("patchsectionmanager/size", width());
}

void PatchSectionManager::mousePressEvent(QMouseEvent *event)
{
    int index = clickedSectionIndex(event);
    if (index >= 0) {
        switchToSection(index);
        if (event->button() == Qt::RightButton)
            popupSectionMenu(index);
    }
    else if (event->button() == Qt::RightButton)
        popupSectionMenu();
}

void PatchSectionManager::mouseDoubleClickEvent(QMouseEvent *event)
{
    int index = clickedSectionIndex(event);
    if (index >= 0) {
        switchToSection(index);
        renameSection();
    }
    else
        TRIGGER_ACTION(ACTION_NEW_PATCH_SECTION);
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
    QString newname = NameChooseDialog::getName(tr("Rename patch section"), tr("New name:"), oldname);
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

void PatchSectionManager::duplicateSection()
{
    int index = patch->currentSectionIndex();
    PatchSection *oldSection = patch->section(index);
    QString newname = NameChooseDialog::getName(
                tr("Duplicate section"),
                tr("New name:"),
                oldSection->getTitle());
    if (newname.isEmpty())
        return;

    Patch *newpatch = new Patch();
    PatchSection *newSection = oldSection->clone();
    newSection->setTitle(newname);
    newpatch->addSection(newSection);
    if (!the_operator->interactivelyRemapRegisters(newpatch)) {
        delete newpatch;
        return;
    }

    PatchSection *newsection = newpatch->section(0)->clone();
    patch->insertSection(index + 1, newsection);
    delete newpatch;
    patch->commit(tr("duplicating section"));
    emit patchModified(); // implies sectionSwitched
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
    int index = patch->currentSectionIndex();
    QString newname = NameChooseDialog::getName(tr("Add new patch section"), tr("Name:"));
    if (newname.isEmpty())
        return;

    patch->insertSection(index + 1, new PatchSection(newname));
    patch->commit(tr("adding new patch section '%1'").arg(newname));
    emit patchModified();
}

void PatchSectionManager::pasteAsSection()
{
    QString newname = NameChooseDialog::getName(tr("Paste as new section"), tr("New section name:"));
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

void PatchSectionManager::rebuildGraphics()
{
    scene()->clear();
    titleViews.clear();

    // Add strut for padding
    scene()->addRect(QRectF(0, 0, viewport()->width(), 0));
    // TODO: kann man das nicht mit setSceneRect() besser machen?

    // Add title
    QGraphicsTextItem *text = scene()->addText(tr("Sections"));
    int textWidth = text->boundingRect().width();
    text->setPos((viewport()->width() - textWidth) / 2, 0);
    text->setDefaultTextColor(COLOR(PSM_COLOR_TITLE));

    int y = text->boundingRect().bottom() + PSM_TOP_PADDING;
    int width = viewport()->width() - 2 * PSM_SIDE_PADDING;

    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSection *section = patch->section(i);
        unsigned numProblems = patch->numProblemsInSection(i);
        PatchSectionTitleView *item = new PatchSectionTitleView(section->getNonemptyTitle(), width, numProblems);
        titleViews.append(item);
        item->setData(DATA_INDEX_SECTION_INDEX, i);
        scene()->addItem(item);
        item->setPos(PSM_SIDE_PADDING, y);
        y += item->boundingRect().height() + PSM_VERTICAL_DISATNCE;
    }

    frameCursor = new FrameCursor();
    scene()->addItem(frameCursor);
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
    patch->switchCurrentSection(i);
    emit sectionSwitched();
}

void PatchSectionManager::switchBackward()
{
    switchToSection(qMax(0, patch->currentSectionIndex() - 1));
}

void PatchSectionManager::switchForward()
{
    switchToSection(qMin(patch->numSections() -1 , patch->currentSectionIndex() + 1));
}
