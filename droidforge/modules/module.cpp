#include "module.h"
#include "tuning.h"

#include <QPainter>

Module::Module(const QString &faceplate)
    : faceplateImage(":images/faceplates/" + faceplate)
    , registerIsHilited{{0}}
{
}

Module::~Module()
{
}

bool Module::isController() const
{
    return data(DATA_INDEX_CONTROLLER_INDEX).isValid();
}

void Module::clearHilites()
{
    memset(&registerIsHilited, 0, sizeof(registerIsHilited));
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
        for (unsigned j=0; j<numRegisters(type); j++) {
            if (registerIsHilited[i][j]) {
                paintHiliteRegister(painter, type, j+1);
            }
        }
    }
}

void Module::hiliteRegisters(bool on, QChar type, unsigned number)
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        if (type == register_types[i] || type == 0) {
            for (unsigned j=0; j<numRegisters(register_types[i]); j++) {
                if (number == j+1+numberOffset(type) || number == 0) {
                    registerIsHilited[i][j] = on;
                }
            }
        }
    }
}

void Module::paintHiliteRegister(QPainter *painter, QChar type, unsigned number)
{
    QRectF r = registerRect(type, number);
    QPen pen(RACV_REGHILITES_PEN_COLOR);
    pen.setWidth(10);
    painter->setPen(pen);
    painter->setBrush(RACV_REGHILITES_BACKGROUND);
    if (type == REGISTER_RGB_LED || type == REGISTER_EXTRA)
        painter->drawRect(r);
    else
        painter->drawEllipse(r);
}


QRectF Module::registerRect(QChar type, unsigned number) const
{
    QPointF posHP = registerPosition(type, number); // in HP, mid
    float size = registerSize(type, number) * RACV_PIXEL_PER_HP;
    QPointF pos(posHP.x() * RACV_PIXEL_PER_HP - size/2,
                posHP.y() * RACV_PIXEL_PER_HP - size/2);
    return QRectF(pos, QSize(size, size));
}

AtomRegister *Module::registerAt(const QPoint &pos) const
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        QChar type = register_types[i];
        if (type == REGISTER_EXTRA)
            continue; // This should not be clickable
        for (unsigned j=0; j<numRegisters(register_types[i]); j++) {
            QRectF r = registerRect(type, j+1);
            if (r.contains(pos)) {
                return registerAtom(type, j+1).clone();
            }
        }
    }
    return 0;
}

AtomRegister Module::registerAtom(QChar type, unsigned number) const
{
    // TODO: This sucks somehow. Too much graphics and logic entanglement.
    unsigned controller = 0;
    if (data(DATA_INDEX_CONTROLLER_INDEX).isValid())
        controller = data(DATA_INDEX_CONTROLLER_INDEX).toInt() + 1;

    return AtomRegister(type, controller, number + numberOffset(type));
}

// TODO: Hier fehlt die Controllernummer!!!!!
void Module::collectAllRegisters(RegisterList &rl, int number) const
{
    for (unsigned i=0; i<NUM_REGISTER_TYPES; i++) {
        QChar type = register_types[i];
        unsigned count = numRegisters(type);
        for (unsigned j=1; j<=count; j++) {
            if (number >= 1)
                rl.append(AtomRegister(type, number, j));
            else
                rl.append(registerAtom(type, j));
        }
    }
}
