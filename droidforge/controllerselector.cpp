#include "controllerselector.h"
#include "modulebuilder.h"
#include "tuning.h"

#include <QGraphicsItem>
#include <QMouseEvent>

ControllerSelector::ControllerSelector(QWidget *parent)
    : QGraphicsView{parent}
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
        qDebug() << gi << "NAME" << name << gi->data(0).toString();
        gi->setPos(x, CSEL_TOP_MARGIN);
        x += module->hp() * CSEL_PIXEL_PER_HP;
        x += CSEL_CONTROLLER_DISTANCE;
    }

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
        if (item)
            selectedController = item->data(0).toString();
        else
            selectedController = "";
    }
}

void ControllerSelector::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (selectedController != "")
        emit controllerSelected(selectedController);
}
