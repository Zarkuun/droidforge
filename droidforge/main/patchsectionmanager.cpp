#include "patchsectionmanager.h"
#include "tuning.h"

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
}

void PatchSectionManager::resizeEvent(QResizeEvent *)
{
    if (patch)
        rebuildGraphics();
}

void PatchSectionManager::mousePressEvent(QMouseEvent *event)
{
    PatchSectionTitleView *tv = 0;
    for (auto item: items(event->pos())) {
        if (item->data(DATA_INDEX_SECTION_INDEX).isValid()) {
            switchToSection(item->data(DATA_INDEX_SECTION_INDEX).toInt());
            break;
        }
    }
}

void PatchSectionManager::setNewPatch(VersionedPatch *newPatch)
{
    qDebug() << "NEUER PATCH" << Q_FUNC_INFO;
    patch = newPatch;
    rebuildGraphics();
    // reconstruct sections, etc.
}

void PatchSectionManager::switchSection(int i)
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
    emit sectionSwitched(i);
}
