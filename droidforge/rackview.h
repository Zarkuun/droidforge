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

    Patch *patch;
    QList<Module *> modules;
    unsigned x;

public:
    explicit RackView();
    void resizeEvent(QResizeEvent *event);
    void setPatch(Patch *patch);

private:
    void addModule(const QString &name);
    void updateGraphics();
    void updateSize();

public slots:
    void addController();
};

#endif // RACKVIEW_H
