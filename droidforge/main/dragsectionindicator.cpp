#include "dragsectionindicator.h"
#include "tuning.h"
#include "colorscheme.h"
#include "globals.h"

#include <QPainter>

QRectF DragSectionIndicator::boundingRect() const
{
    QRectF r = paintRect();
    if (insertPosition < r.top())
        r.setTop(insertPosition - PSM_SECTION_DRAG_WIDTH);
    else if (insertPosition > r.bottom())
        r.setBottom(insertPosition + PSM_SECTION_DRAG_WIDTH);
    return r;
}

void DragSectionIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth

    QRectF pr = paintRect();

    QPen pen;
    pen.setColor(hits
                ? COLOR(RACV_COLOR_DRAGARROW)
                : COLOR(RACV_COLOR_DRAGARROW_UNLOCKED));
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setWidth(PSM_SECTION_DRAG_WIDTH);
    painter->setPen(pen);
    painter->drawRect(pr);

    if (!hits) {
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
    }

    painter->drawLine(pr.left(), insertPosition, pr.right(), insertPosition);

    if (insertPosition >= pr.top() && insertPosition <= pr.bottom())
        return;

    float x = (pr.left() + pr.right()) / 2;
    float y;
    float phi;
    if (insertPosition < pr.top()) {
        y = pr.top();
        phi = 180;
    }
    else {
        y = pr.bottom();
        phi = 0;
    }

    painter->drawLine(x, insertPosition, x, y);
    painter->translate(x, insertPosition);
    painter->rotate(phi);
    paintArrowHead(painter, PSM_ARROW_HEAD_SIZE);
}

void DragSectionIndicator::setSectionRect(QRectF rect)
{
    sectionRect = rect;
}

void DragSectionIndicator::setInsertPos(float yPos, bool h)
{
    insertPosition = yPos;
    hits = h;
}

QRectF DragSectionIndicator::paintRect() const
{
    return sectionRect.adjusted(
        -PSM_SECTION_DRAG_WIDTH,
        -PSM_SECTION_DRAG_WIDTH,
         PSM_SECTION_DRAG_WIDTH,
         PSM_SECTION_DRAG_WIDTH);
}
