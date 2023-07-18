#include "module.h"
#include "colorscheme.h"
#include "globals.h"
#include "tuning.h"
#include "editoractions.h"
#include "mainwindow.h"

#include <QPainter>
#include <QGraphicsScene>

Module::Module(MainWindow *mainWindow, const QString &name)
    : mainWindow(mainWindow)
    , name(name)
    , faceplateImage(":images/faceplates/" + name)
    , registerHilite{{0}}
    , registerLabels(0)
    , pixelHeight(400)
{
}
Module::~Module()
{
}
bool Module::isController() const
{
    return data(DATA_INDEX_CONTROLLER_INDEX).isValid();
}
unsigned Module::controllerNumber() const
{
    unsigned controller = 0;
    if (data(DATA_INDEX_CONTROLLER_INDEX).isValid())
        controller = data(DATA_INDEX_CONTROLLER_INDEX).toInt() + 1;
    return controller;
}
unsigned Module::g8Number() const
{
    unsigned g8 = 0;
    if (data(DATA_INDEX_G8_NUMBER).isValid())
        g8 = data(DATA_INDEX_G8_NUMBER).toInt();
    return g8;
}
void Module::createRegisterItems(QGraphicsScene *scene, int moduleIndex, int controllerIndex, unsigned g8Number)
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        register_type_t type = register_types[i];
        if (type == REGISTER_EXTRA || type == REGISTER_LED || type == REGISTER_NORMALIZE)
            continue; // This should not be clickable
        for (unsigned j=0; j<numRegisters(register_types[i]); j++) {
            QRectF rect = registerRect(type, j+1, rectAspect(type), 1).translated(pos().x(), 0);
            auto item = scene->addRect(rect, QPen(QColor(0, 0, 0, 0), 0));
            item->setData(DATA_INDEX_DRAGGER_PRIO, 2);
            item->setData(DATA_INDEX_REGISTER_NAME, registerAtom(type,  j+1).toString());
            item->setData(DATA_INDEX_MODULE_INDEX, moduleIndex);
            item->setData(DATA_INDEX_CONTROLLER_INDEX, controllerIndex);
            item->setData(DATA_INDEX_G8_NUMBER, g8Number);
        }
    }
}
void Module::clearHilites()
{
    memset(&registerHilite, 0, sizeof(registerHilite));
}
QRectF Module::boundingRect() const
{
    return moduleRect().adjusted(-20, 0, 20, 0);
}
QRectF Module::moduleRect() const
{
    return QRectF(0, 0, hp() * RACV_PIXEL_PER_HP, RACV_MODULE_HEIGHT);
}
void Module::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    QRect r = moduleRect().toRect();

    // TODO: Das hier auf Windows ausprobieren und eventuell cachen
    QImage scaledImage = faceplateImage.scaledToHeight(pixelHeight, Qt::SmoothTransformation);
    painter->drawImage(r, scaledImage);

    paintRegisterHilites(painter);
    paintRegisterLabels(painter);
}
void Module::paintRegisterHilites(QPainter *painter)
{
    if (!ACTION(ACTION_SHOW_REGISTER_USAGE)->isChecked())
        return;

    paintRegisterHilites(painter, 1);
    paintRegisterHilites(painter, 2);
}
void Module::paintRegisterHilites(QPainter *painter, int usage)
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        register_type_t type = register_types[i];
        for (unsigned j=0; j<numRegisters(type); j++) {
            if (usage == registerHilite[i][j]) {
                paintHiliteRegister(painter, usage, type, j+1);
            }
        }
    }
}
void Module::paintHiliteRegister(QPainter *painter, int usage, register_type_t type, unsigned number)
{
    float ra = rectAspect(type);
    QRectF r = registerRect(type, number, ra, 1); // usage);
    QPen pen;

    int d = 8;
    int i=0;
    while (r.width() >= 0 ) {
        i++;
        if (i%2 == 0)
            pen.setColor(COLOR(RACV_REGHILITES_LESSER_PEN_COLOR));
        else if (usage == 2)
            pen.setColor(COLOR(RACV_REGHILITES_PEN_COLOR));
        else
            pen.setColor(QColor(0, 0, 0));

        pen.setWidth(d);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        if (ra)
            painter->drawRect(r);
        else
            painter->drawEllipse(r);
        r.adjust(d, d, -d, -d);
    }
}
void Module::paintRegisterLabels(QPainter *painter)
{
    if (!registerLabels)
        return;

    if (!ACTION(ACTION_SHOW_REGISTER_LABELS)->isChecked())
        return;

    painter->setPen(COLOR(RACV_COLOR_REGISTER_LABEL));
    unsigned controller = controllerNumber();
    unsigned g8 = g8Number();

    QMapIterator<AtomRegister, RegisterLabel> it(*registerLabels);
    while (it.hasNext()) {
        it.next();
        AtomRegister atom = it.key();
        if (atom.getController() != controller)
            continue;
        if (atom.getG8Number() != g8)
            continue;
        if (!haveRegister(atom))
            continue;
        paintRegisterLabel(painter, atom, it.value());
    }
}
void Module::paintRegisterLabel(QPainter *painter, AtomRegister atom, const RegisterLabel &label)
{
    QString text = label.shorthand;
    if (text == "")
        text = label.description.mid(0, 3); // MAX_LENGTH_SHORTHAND);

    register_type_t regtype = atom.getRegisterType();
    unsigned regnum = atom.getNumber() - numberOffset(regtype);

    QRectF rr = registerRect(regtype, regnum, rectAspect(regtype), 1);
    qreal mid = (rr.left() + rr.right()) / 2;
    float dist = RACV_PIXEL_PER_HP * labelDistance(regtype, regnum);
    float width = RACV_PIXEL_PER_HP * labelWidth(regtype, regnum);

    QRectF r(mid - width / 2, rr.bottom() + dist,
             width, RACV_LABEL_HEIGHT);

    painter->setPen(QColor(0, 0, 0));
    painter->setBrush(QColor(255, 255, 255));
    painter->drawRoundedRect(r, 10, 10);
    QFont font;
    font.setPixelSize(50);
    painter->setFont(font);
    painter->drawText(r, Qt::TextSingleLine | Qt::AlignCenter | Qt::AlignVCenter, text);
}
bool Module::haveRegister(AtomRegister atom)
{
    unsigned count = numRegisters(atom.getRegisterType());
    unsigned offset = numberOffset(atom.getRegisterType());
    bool have = atom.getNumber() >= 1 + offset && atom.getNumber() <= count + offset;
    return have;
}
void Module::hiliteRegisters(int usage, register_type_t type, unsigned number)
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        if (type == register_types[i] || type == 0) {
            for (unsigned j=0; j<numRegisters(register_types[i]); j++) {
                if (number == j+1+numberOffset(type) || number == 0) {
                    registerHilite[i][j] = usage;
                }
            }
        }
    }
}
QRectF Module::registerRect(register_type_t type, unsigned number, float aspect, int usage) const
{
    QPointF posHP = registerPosition(type, number); // in HP, mid
    float size = registerSize(type, number) * RACV_PIXEL_PER_HP * 2 / 3;
    if (usage == 2)
        size = size * 5 / 4;
    float ysize = aspect == 0.0 ? size : size * aspect;
    QPointF pos(posHP.x() * RACV_PIXEL_PER_HP - size/2,
                posHP.y() * RACV_PIXEL_PER_HP - ysize/2);
    return QRectF(pos, QSize(size, ysize));
}
AtomRegister *Module::registerAt(const QPoint &pos) const
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        register_type_t type = register_types[i];
        if (type == REGISTER_EXTRA)
            continue; // This should not be clickable
        for (unsigned j=0; j<numRegisters(register_types[i]); j++) {
            QRectF r = registerRect(type, j+1, rectAspect(type), 1);
            if (r.contains(pos)) {
                return registerAtom(type, j+1).clone();
            }
        }
    }
    return 0;
}
AtomRegister Module::registerAtom(register_type_t type, unsigned number) const
{
    return AtomRegister(type, controllerNumber(), g8Number(), number + numberOffset(type));
}
void Module::collectAllRegisters(RegisterList &rl) const
{
    for (unsigned i=0; i<NUM_REGISTER_TYPES; i++) {
        register_type_t type = register_types[i];
        unsigned count = numRegisters(type);
        for (unsigned number=1; number<=count; number++)
            rl.append(registerAtom(type, number));
    }
}
