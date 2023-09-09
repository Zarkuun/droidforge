#ifndef SECTIONCOMMENTVIEW_H
#define SECTIONCOMMENTVIEW_H

#include <QGraphicsItem>
#include <QStringList>

class SectionCommentView : public QObject, public QGraphicsItem
{
    QStringList comment;
    float lineHeight;
    float totalWidth;

public:
    SectionCommentView(const QStringList &comment, float width, float lineHeight);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
};

#endif // SECTIONCOMMENTVIEW_H
