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

void Module::clearHilites()
{
    memset(&controlHilit, 0, sizeof(controlHilit));
}

QRectF Module::boundingRect() const
{
    return QRectF(0, 0, hp() * RACV_PIXEL_PER_HP, RACV_MODULE_HEIGHT);
}

void Module::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRect r = boundingRect().toRect();
    painter->drawPixmap(r, faceplateImage);

    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        QChar type = register_types[i];
        for (unsigned j=0; j<numControls(type); j++) {
            if (controlHilit[i][j]) {
                paintHiliteControl(painter, type, j+1);
            }
        }
    }
}

void Module::hiliteControls(bool on, QChar type, unsigned number)
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        if (type == register_types[i] || type == 0) {
            for (unsigned j=0; j<numControls(register_types[i]); j++) {
                if (number == j+1 || number == 0) {
                    controlHilit[i][j] = on;
                }
            }
        }
    }
}

void Module::paintHiliteControl(QPainter *painter, QChar type, unsigned number)
{
    QRectF r = registerRect(type, number);
    QPen pen(QColor(255, 255, 0));
    pen.setWidth(10);
    painter->setPen(pen);
    if (type == REGISTER_RGB || type == REGISTER_EXTRA)
        painter->drawRect(r);
    else
        painter->drawEllipse(r);
}


QRectF Module::registerRect(QChar type, unsigned number) const
{
    QPointF posHP = controlPosition(type, number); // in HP, mid
    float size = controlSize(type, number) * RACV_PIXEL_PER_HP;
    QPointF pos(posHP.x() * RACV_PIXEL_PER_HP - size/2,
                posHP.y() * RACV_PIXEL_PER_HP - size/2);
    return QRectF(pos, QSize(size, size));
}

AtomRegister *Module::registerAt(const QPoint &pos) const
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        QChar type = register_types[i];
        for (unsigned j=0; j<numControls(register_types[i]); j++) {
            QRectF r = registerRect(type, j+1);
            if (r.contains(pos)) {
                return registerAtom(type, j+1);
            }
        }
    }
    return 0;
}

AtomRegister *Module::registerAtom(QChar type, unsigned number) const
{
    unsigned controller = 0;
    if (data(0).isValid())
        controller = data(0).toInt();

    return new AtomRegister(type, controller, number);
}
