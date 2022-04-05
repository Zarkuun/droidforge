#include "rackview.h"
#include <QGraphicsItem>

RackView::RackView(QWidget *parent)
: QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    scene->setBackgroundBrush(QColor(20, 20, 20));
    QPixmap *image1 = new QPixmap(":images/faceplates/faceplate-master-on.jpg");
    QPixmap *image2 = new QPixmap(":images/faceplates/faceplate-g8-on.jpg");
    QPixmap *image3 = new QPixmap(":images/faceplates/faceplate-b32-white.jpg");

    QGraphicsItem *pm1 = scene->addPixmap(*image1);
    QGraphicsItem *pm2 = scene->addPixmap(*image2);
    QGraphicsItem *pm3 = scene->addPixmap(*image3);
    pm1->setPos(0, 0);
    pm2->setPos(720, 0);
    pm3->setPos(1100, 0);

    scale(0.1, 0.1);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setScene(scene);
}
