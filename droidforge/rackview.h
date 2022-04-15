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
    explicit RackView(const Patch *patch);
    void resizeEvent(QResizeEvent *event);

private:
    void buildRack(const Patch *);
};

#endif // RACKVIEW_H
