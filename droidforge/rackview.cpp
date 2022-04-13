#include "rackview.h"
#include "modulebuilder.h"
#include "tuning.h"

#include <QGraphicsItem>
#include <QResizeEvent>

RackView::RackView(Rack *arack)
    : QGraphicsView()
    , rack(arack)
{
    setMinimumHeight(MIN_RACK_HEIGHT);
    setMaximumHeight(MAX_RACK_HEIGHT);

    QGraphicsScene *thescene = new QGraphicsScene();
    thescene->setBackgroundBrush(COLOR_RACK_BACKGROUND);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(thescene);

    buildRack();

    fitInView(thescene->sceneRect(), Qt::KeepAspectRatio);
}


void RackView::resizeEvent(QResizeEvent *)
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}


void RackView::buildRack()
{
    QListIterator<Module *> i = rack->iterator();
    unsigned x = 0;
    while (i.hasNext()) {
        Module *module = i.next();
        QPixmap *image = new QPixmap(QString(":images/faceplates/" + module->faceplate()));
        QGraphicsItem *gi = scene()->addPixmap(*image);
        gi->setPos(x, 0);
        x += module->hp() * 88;
    }
}
