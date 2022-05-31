#include "patchsectionmanager.h"
#include "tuning.h"
#include "updatehub.h"
#include "editoractions.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QSettings>
#include <QResizeEvent>

#define DATA_INDEX_SECTION_INDEX 0

PatchSectionManager::PatchSectionManager(QWidget *parent)
    : QGraphicsView{parent}
    , patch(0)
{
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumWidth(100);
    setMaximumWidth(500);
    setScene(new QGraphicsScene());
    scene()->setBackgroundBrush(QColor(PSM_COLOR_BACKGROUND));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMouseTracking(true);

    // Actions we handle
    CONNECT_ACTION(ACTION_PREVIOUS_SECTION, &PatchSectionManager::switchBackward);
    CONNECT_ACTION(ACTION_NEXT_SECTION, &PatchSectionManager::switchForward);

    // Events that we create
    connect(this, &PatchSectionManager::sectionSwitched, the_hub, &UpdateHub::switchSection);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &PatchSectionManager::switchSection);
    connect(the_hub, &UpdateHub::patchChanged, this, &PatchSectionManager::changePatch);
}

void PatchSectionManager::resizeEvent(QResizeEvent *)
{
    if (patch)
        rebuildGraphics();
}

void PatchSectionManager::mousePressEvent(QMouseEvent *event)
{
    for (auto item: items(event->pos())) {
        if (item->data(DATA_INDEX_SECTION_INDEX).isValid()) {
            switchToSection(item->data(DATA_INDEX_SECTION_INDEX).toInt());
            break;
        }
    }
}

void PatchSectionManager::changePatch(VersionedPatch *newPatch)
{
    qDebug() << "NEUER PATCH" << Q_FUNC_INFO;
    patch = newPatch;
    rebuildGraphics();
    // reconstruct sections, etc.
}

void PatchSectionManager::switchSection()
{
    updateCursor();
}

void PatchSectionManager::rebuildGraphics()
{
    scene()->clear();
    titleViews.clear();

    // Add strut for paddinbg
    scene()->addRect(QRectF(0, 0, viewport()->width(), 0));

    int y = PSM_TOP_PADDING;
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
    PatchSectionTitleView *tv = titleViews[patch->currentSectionIndex()];
    frameCursor->setRect(tv->boundingRect().translated(tv->pos()));
    frameCursor->startAnimation();
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
