#include "patchsectionmanager.h"
#include "clipboard.h"
#include "tuning.h"
#include "updatehub.h"
#include "editoractions.h"
#include "namechoosedialog.h"
#include "interactivepatchoperation.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QSettings>
#include <QResizeEvent>
#include <QMenu>

#define DATA_INDEX_SECTION_INDEX 0

PatchSectionManager::PatchSectionManager(QWidget *parent)
    : QGraphicsView{parent}
    , patch(0)
    , lastIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumWidth(150);
    setMaximumWidth(300);
    setScene(new QGraphicsScene());
    scene()->setBackgroundBrush(QColor(PSM_COLOR_BACKGROUND));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMouseTracking(true);

    // Actions we handle
    connectActions();

    // Events we create
    connect(this, &PatchSectionManager::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchSectionManager::sectionSwitched, the_hub, &UpdateHub::switchSection);

    // Events we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &PatchSectionManager::switchSection);
    connect(the_hub, &UpdateHub::patchChanged, this, &PatchSectionManager::changePatch);
    connect(the_hub, &UpdateHub::patchModified, this, &PatchSectionManager::modifyPatch);
}

void PatchSectionManager::connectActions()
{
    CONNECT_ACTION(ACTION_PREVIOUS_SECTION, &PatchSectionManager::switchBackward);
    CONNECT_ACTION(ACTION_NEXT_SECTION, &PatchSectionManager::switchForward);
    CONNECT_ACTION(ACTION_NEW_PATCH_SECTION, &PatchSectionManager::newSectionAfterCurrent);
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
    if (index >= 0) {
        ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_WITH_PREVIOUS_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_WITH_NEXT_SECTION, menu);
        ADD_ACTION_IF_ENABLED(ACTION_MERGE_ALL_SECTIONS, menu);
    }
    ADD_ACTION_IF_ENABLED(ACTION_CREATE_SECTION_FROM_SELECTION, menu);
    menu->popup(QCursor::pos());
}

void PatchSectionManager::resizeEvent(QResizeEvent *)
{
    if (patch)
        rebuildGraphics();
}

void PatchSectionManager::mousePressEvent(QMouseEvent *event)
{
    int index = clickedSectionIndex(event);
    if (index >= 0) {
        switchToSection(index);
        if (event->button() == Qt::RightButton)
            popupSectionMenu(index);
    }
    else
        popupSectionMenu();
}

void PatchSectionManager::mouseDoubleClickEvent(QMouseEvent *event)
{
    int index = clickedSectionIndex(event);
    if (index >= 0) {
        switchToSection(index);
        renameSection();
    }
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

void PatchSectionManager::changePatch(VersionedPatch *newPatch)
{
    patch = newPatch;
    rebuildGraphics();
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
    newpatch->addSection(oldSection->clone());
    if (!InteractivePatchOperation::interactivelyRemapRegisters(patch, newpatch)) {
        delete newpatch;
        return;
    }

    PatchSection *newsection = newpatch->section(0)->clone();
    patch->insertSection(index + 1, newsection);
    patch->switchCurrentSection(index + 1);
    delete newpatch;
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
    QString newname = NameChooseDialog::getName(tr("Add new patch section"), tr("Name:"), SECTION_DEFAULT_NAME);
    if (newname.isEmpty())
        return;

    patch->insertSection(index + 1, new PatchSection(newname));
    patch->switchCurrentSection(index + 1);
    patch->commit(tr("adding new patch section '%1'").arg(newname));
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

    // Add title
    QGraphicsTextItem *text = scene()->addText(tr("Sections"));
    int textWidth = text->boundingRect().width();
    text->setPos((viewport()->width() - textWidth) / 2, 0);
    text->setDefaultTextColor(PSM_COLOR_TITLE);

    int y = text->boundingRect().bottom() + PSM_TOP_PADDING;
    int width = viewport()->width() - 2 * PSM_SIDE_PADDING;

    for (qsizetype i=0; i<patch->numSections(); i++) {
        PatchSectionTitleView *item = new PatchSectionTitleView(patch->section(i)->getNonemptyTitle(), width);
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
    qDebug() << "switche to " << i;
    patch->switchCurrentSection(i);
    emit sectionSwitched();
}

void PatchSectionManager::switchBackward()
{
    switchToSection((patch->currentSectionIndex() - 1 + patch->numSections()) % patch->numSections());
}

void PatchSectionManager::switchForward()
{
    switchToSection((patch->currentSectionIndex() + 1) % patch->numSections());
}
