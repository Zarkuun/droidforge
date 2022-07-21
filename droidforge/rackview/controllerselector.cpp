#include "controllerselector.h"
#include "colorscheme.h"
#include "modulebuilder.h"
#include "tuning.h"

#include <QGraphicsItem>
#include <QMouseEvent>

ControllerSelector::ControllerSelector(QWidget *parent)
    : QGraphicsView{parent}
    , selectedController("p2b8")
{
    QGraphicsScene *scene = new QGraphicsScene();
    const QStringList &controllers = ModuleBuilder::allControllers();
    int x = 0;
    setBackgroundBrush(COLOR(CSEL_COLOR_BACKGROUND));

    int height = 0;
    for (qsizetype i=0; i<controllers.size(); i++)
    {
        QString name = controllers[i];
        Module *module = ModuleBuilder::buildModule(name);
        scene->addItem(module);
        module->setData(0, name);
        module->setZValue(10); // make it above margin rect
        module->setPos(x, 0); // CSEL_TOP_MARGIN);
        x += module->hp() * CSEL_PIXEL_PER_HP;
        x += CSEL_CONTROLLER_DISTANCE;
        height = module->boundingRect().height();
    }

    QPen pen(COLOR(CSEL_COLOR_CURSOR));
    pen.setWidth(CSEL_CURSOR_WIDTH);
    cursor = scene->addRect(0, 0, 0, 0, pen);
    cursor->setZValue(20);
    placeCursor();

    // Force visible margins around everything
    scene->setSceneRect(
                - CSEL_SIDE_MARGIN,
                - CSEL_TOP_MARGIN,
                x - CSEL_CONTROLLER_DISTANCE + 2 * CSEL_SIDE_MARGIN,
                height + 2 * CSEL_TOP_MARGIN);

    setScene(scene);

    // This widget must not have focus. Otherwise a clock on
    // one of the controllers will move the focus from the dialog
    // away to here and the keyboard will stop working.
    setFocusPolicy(Qt::NoFocus);
}
void ControllerSelector::resizeEvent(QResizeEvent *)
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}
void ControllerSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::MouseButtonPress) {
        QGraphicsItem *item = itemAt(event->pos());
        if (item) {
            // The cursor wire frame is also clickable and
            // will appear here - shadowing the actual
            // controller below it. So make sure we really
            // have a valid controller name.
            QString cname = item->data(0).toString();
            if (!cname.isEmpty()) {
                selectedController = cname;
                placeCursor();
            }
        }
    }
}
void ControllerSelector::mouseDoubleClickEvent(QMouseEvent *)
{
    if (!selectedController.isEmpty()) {
        emit controllerSelected(selectedController);
    }
}
void ControllerSelector::placeCursor()
{
    int x = 0;
    const QStringList &controllers = ModuleBuilder::allControllers();
    for (qsizetype i=0; i<controllers.size(); i++)
    {
        QString name = controllers[i];
        Module *module = ModuleBuilder::buildModule(name);
        if (name == selectedController) {
            cursor->setRect(module->moduleRect().translated(x, 0)
                            .adjusted(-CSEL_CURSOR_WIDTH,
                                      -CSEL_CURSOR_WIDTH,
                                       CSEL_CURSOR_WIDTH,
                                       CSEL_CURSOR_WIDTH));
            cursor->update();
        }
        x += module->hp() * CSEL_PIXEL_PER_HP;
        x += CSEL_CONTROLLER_DISTANCE;
        delete module;
    }
}
void ControllerSelector::moveCursor(int whence)
{
    const QStringList &controllers = ModuleBuilder::allControllers();
    int count = controllers.size();
    int sel = 0;
    for (qsizetype i=0; i<count; i++) {
        if (selectedController == controllers[i]) {
            sel = i;
            break;
        }
    }
    selectedController = controllers[(sel + whence + count) % count];
    placeCursor();
}
