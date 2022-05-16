#include "controllerselector.h"
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
    int x = CSEL_SIDE_MARGIN;

    for (qsizetype i=0; i<controllers.size(); i++)
    {
        QString name = controllers[i];
        Module *module = ModuleBuilder::buildModule(name);
        QPixmap *image = module->faceplateImage();
        QGraphicsItem *gi = scene->addPixmap(*image);
        gi->setData(0, name);
        gi->setZValue(10); // make it above margin rect
        gi->setPos(x, CSEL_TOP_MARGIN);
        x += module->hp() * CSEL_PIXEL_PER_HP;
        x += CSEL_CONTROLLER_DISTANCE;
    }

    QPen pen(COLOR_FRAME_CURSOR);
    pen.setWidth(CSEL_CURSOR_WIDTH);
    cursor = scene->addRect(0, 0, 0, 0, pen);
    cursor->setZValue(20);
    placeCursor();

    // Force visible margins around everything
    scene->addRect(
                0,  0,
                x - CSEL_CONTROLLER_DISTANCE + 2 * CSEL_SIDE_MARGIN,
                CSEL_HEIGHT),
            QPen(QColor(0, 0, 0, 0));

    setScene(scene);
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
            selectedController = item->data(0).toString();
            placeCursor();
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
    int x = CSEL_SIDE_MARGIN;
    const QStringList &controllers = ModuleBuilder::allControllers();
    for (qsizetype i=0; i<controllers.size(); i++)
    {
        QString name = controllers[i];
        Module *module = ModuleBuilder::buildModule(name);
        if (name == selectedController) {
            QRectF cursorRect(x - CSEL_CURSOR_WIDTH/2,
                            CSEL_TOP_MARGIN - CSEL_CURSOR_WIDTH/2,
                            module->hp() * CSEL_PIXEL_PER_HP +
                            CSEL_CURSOR_WIDTH * 2,
                            CSEL_CURSOR_HEIGHT);
            cursor->setRect(cursorRect);
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
