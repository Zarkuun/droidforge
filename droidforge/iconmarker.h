#ifndef ICONMARKER_H
#define ICONMARKER_H

#include "cursorposition.h"

#include <QGraphicsItem>

#define DATA_INDEX_ICON_MARKER 1

typedef enum {
    ICON_MARKER_PROBLEM,
    ICON_MARKER_INFO,
    ICON_MARKER_LEDMISMATCH,
    ICON_MARKER_FOLDED,
} icon_marker_t;

class IconMarker : public QGraphicsItem
{
    icon_marker_t type;
    CursorPosition position;
    const QImage &image;

public:
    IconMarker(const CursorPosition &pos, icon_marker_t type, const QString &toolTip);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    const CursorPosition &cursorPosition() const { return position; };
    icon_marker_t getType() const { return type; };

private:
    QString iconName() const;
    int size() const;
};


#endif // ICONMARKER_H
