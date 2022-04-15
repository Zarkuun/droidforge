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
    unsigned x = 0;
    for (qsizetype i=0; i<patch->numControllers(); i++)
    {
        QString name = patch->controller(i);
        Module *module = ModuleBuilder::buildModule(name);
        QPixmap *image = new QPixmap(QString(":images/faceplates/" + module->faceplate()));
        QGraphicsItem *gi = scene()->addPixmap(*image);
        gi->setPos(x, 0);
        x += module->hp() * PIXEL_PER_HP;
    }
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}
