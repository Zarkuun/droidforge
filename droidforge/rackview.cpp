#include "rackview.h"
#include "modulebuilder.h"
#include "tuning.h"

#include <QGraphicsItem>
#include <QResizeEvent>

RackView::RackView()
    : QGraphicsView()
{
    setMinimumHeight(MIN_RACK_HEIGHT);
    setMaximumHeight(MAX_RACK_HEIGHT);

    QGraphicsScene *thescene = new QGraphicsScene();
    thescene->setBackgroundBrush(COLOR_RACK_BACKGROUND);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);
}


void RackView::resizeEvent(QResizeEvent *)
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}


void RackView::setPatch(const Patch *patch)
{
    scene()->clear();
    if (!patch)
        return;

    x = 0;
    addModule("master");
    if (patch->needG8())
        addModule("g8");
    if (patch->needX7()) {
        qDebug() << "XY";
        addModule("x7");
    }

    for (qsizetype i=0; i<patch->numControllers(); i++)
        addModule(patch->controller(i));
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void RackView::addModule(const QString &name)
{
    Module *module = ModuleBuilder::buildModule(name);
    QPixmap *image = new QPixmap(QString(":images/faceplates/" + module->faceplate()));
    QGraphicsItem *gi = scene()->addPixmap(*image);
    gi->setPos(x, 0);
    x += module->hp() * PIXEL_PER_HP;
}
