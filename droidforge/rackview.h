#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "module.h"
#include "patch.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>


class RackView : public QGraphicsView
{
    Q_OBJECT

    QList<Module *> modules;

public:
    explicit RackView();
    void resizeEvent(QResizeEvent *event);
    void setPatch(const Patch *patch);
};

#endif // RACKVIEW_H
