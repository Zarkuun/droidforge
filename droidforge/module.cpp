#include "module.h"
#include "tuning.h"

#include <QPainter>

Module::Module(const QString &faceplate)
    : faceplateImage(":images/faceplates/" + faceplate)
    , controlHilit{{0}}
{
}

Module::~Module()
{
}

void Module::hiliteControls(bool on, QChar type, unsigned number)
{
    qDebug() << "HI" << on << type << number;
    for (int i=0; i<NUM_CONTROL_TYPES; i++) {
        if (type == register_types[i] || type == 0) {
            qDebug() << name();
            for (unsigned j=0; j<numControls(register_types[i]); j++) {
                qDebug("-");
                if (number == j+1 || number == 0) {
                    qDebug("x");
                    controlHilit[i][j] = on;
                }
            }
        }
    }
    update();
}

QRectF Module::boundingRect() const
{
    return QRectF(0, 0, hp() * RACV_PIXEL_PER_HP, RACV_MODULE_HEIGHT);
}

void Module::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRect r = boundingRect().toRect();
    painter->drawPixmap(r, faceplateImage);

    for (int i=0; i<NUM_CONTROL_TYPES; i++) {
        QChar type = register_types[i];
        for (unsigned j=0; j<numControls(type); j++) {
            if (controlHilit[i][j]) {
                paintHiliteControl(painter, type, j+1);
            }
        }
    }
}

void Module::paintHiliteControl(QPainter *painter, QChar type, unsigned number)
{
    QPointF posHP = controlPosition(type, number); // in HP, mid
    float size = controlSize(type, number) * RACV_PIXEL_PER_HP;
    QPointF pos(posHP.x() * RACV_PIXEL_PER_HP - size/2,
                posHP.y() * RACV_PIXEL_PER_HP - size/2);
    QRectF r(pos, QSize(size, size));
    QPen pen(QColor(255, 255, 0));
    pen.setWidth(10);
    painter->setPen(pen);
    if (type == 'R' || type == 'X')
        painter->drawRect(r);
    else
        painter->drawEllipse(r);
}
