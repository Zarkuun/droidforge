#include "module.h"
#include "tuning.h"

#include <QPainter>

Module::Module(const QString &faceplate)
    : faceplateImage(":images/faceplates/" + faceplate)
{
}

Module::~Module()
{
}

QRectF Module::boundingRect() const
{
    return QRectF(0, 0, hp() * RACV_PIXEL_PER_HP, RACV_MODULE_HEIGHT);
}

void Module::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRect r = boundingRect().toRect();
    painter->drawPixmap(r, faceplateImage);
}
