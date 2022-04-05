#include "rackview.h"
#include "modulebuilder.h"

#include <QGraphicsItem>
#include <QResizeEvent>

RackView::RackView(Rack *arack)
    : QGraphicsView()
    , rack(arack)
{
    scene = new QGraphicsScene();
    scene->setBackgroundBrush(QColor(20, 20, 20));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(scene);

    buildRack();

    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}


void RackView::resizeEvent(QResizeEvent *)
{
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void RackView::buildRack()
{
    QListIterator<Module *> i = rack->iterator();
    unsigned x = 0;
    while (i.hasNext()) {
        Module *module = i.next();
        QPixmap *image = new QPixmap(QString(":images/faceplates/" + module->faceplate()));
        QGraphicsItem *gi = scene->addPixmap(*image);
        gi->setPos(x, 0);
        x += module->hp() * 88;
    }
}
