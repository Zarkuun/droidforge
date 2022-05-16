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
    unsigned x;

public:
    explicit RackView();
    void resizeEvent(QResizeEvent *event);
    void setPatch(const Patch *patch);

private:
    void addModule(const QString &name);
};

#endif // RACKVIEW_H
