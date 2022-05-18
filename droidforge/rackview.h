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
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hiliteRegisters(const RegisterList &registers);

private:
    void addModule(const QString &name, int controllerIndex=-1);
    void updateGraphics();
    void updateSize();
    void popupContextMenu(int controller);

public slots:
    void addController();

private slots:
    void removeController(int controller);
    void moveController(int oldindex, int newindex);
};

#endif // RACKVIEW_H
