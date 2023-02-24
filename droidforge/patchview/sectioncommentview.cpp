#include "sectioncommentview.h"
#include "colorscheme.h"
#include "tuning.h"
#include "globals.h"

#include <QPainter>


SectionCommentView::SectionCommentView(const QStringList &comment, float width, float lineHeight)
    : comment(comment)
    , lineHeight(lineHeight)
    , totalWidth(width)
{
}

QRectF SectionCommentView::boundingRect() const
{
    float height = comment.count() * lineHeight + 2 * PSV_COMMENT_PADDING;
    return QRectF(0, 0, totalWidth, height);
}

void SectionCommentView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    painter->fillRect(boundingRect(), COLOR(PSV_COLOR_COMMENT_BG));


    QRectF textRect(CIRV_TEXT_SIDE_PADDING,
                    PSV_COMMENT_PADDING,
                    boundingRect().width() - 2 * CIRV_TEXT_SIDE_PADDING,
                    boundingRect().height() - 2 * PSV_COMMENT_PADDING);

    painter->setPen(COLOR(PSV_COLOR_COMMENT_TEXT));
    painter->drawText(
                textRect,
                Qt::AlignLeft | Qt::AlignJustify | Qt::AlignTop,
                comment.join("\n"));
}
