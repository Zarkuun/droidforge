#ifndef PATCHSECTIONTITLEVIEW_H
#define PATCHSECTIONTITLEVIEW_H

#include <QGraphicsItem>

class PatchSectionTitleView : public QGraphicsItem
{
    QString title;
    int width;

public:
    PatchSectionTitleView(const QString title, int width);
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
};

#endif // PATCHSECTIONTITLEVIEW_H
