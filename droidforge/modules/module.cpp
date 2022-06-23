#include "module.h"
#include "colorscheme.h"
#include "tuning.h"

#include <QPainter>

Module::Module(const QString &faceplate)
    : faceplateImage(":images/faceplates/" + faceplate)
    , registerHilite{{0}}
    , registerLabels(0)
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
void Module::clearHilites()
{
    memset(&registerHilite, 0, sizeof(registerHilite));
}
QRectF Module::boundingRect() const
{
    return QRectF(0, 0, hp() * RACV_PIXEL_PER_HP, RACV_MODULE_HEIGHT);
}
void Module::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRect r = boundingRect().toRect();
    painter->drawPixmap(r, faceplateImage);

    paintRegisterHilites(painter);
    paintRegisterLabels(painter);
}
void Module::paintRegisterHilites(QPainter *painter)
{
    for (int i=0; i<NUM_REGISTER_TYPES; i++) {
        QChar type = register_types[i];
        for (unsigned j=0; j<numRegisters(type); j++) {
            if (registerHilite[i][j]) {
                paintHiliteRegister(painter, registerHilite[i][j], type, j+1);
            }
        }
    }
}
void Module::paintHiliteRegister(QPainter *painter, int usage, QChar type, unsigned number)
{
    QRectF r = registerRect(type, number, usage);
    QPen pen(COLOR(RACV_REGHILITES_PEN_COLOR));
    pen.setWidth(10);
    painter->setPen(pen);
    painter->setBrush(usage == 2 ? COLOR(RACV_REGHILITES_BG) : COLOR(RACV_REGHILITES_INACTIVE_BG));
    if (type == REGISTER_RGB_LED || type == REGISTER_EXTRA)
        painter->drawRect(r);
    else
        painter->drawEllipse(r);
}
void Module::paintRegisterLabels(QPainter *painter)
{
    if (!registerLabels)
        return;

    painter->setPen(COLOR(RACV_COLOR_REGISTER_LABEL));
    unsigned controller = controllerNumber();

    QMapIterator<AtomRegister, RegisterLabel> it(*registerLabels);
    while (it.hasNext()) {
        it.next();
        AtomRegister atom = it.key();
        if (atom.controller() != controller)
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

    QRectF r = registerRect(atom.getRegisterType(), atom.getNumber() - numberOffset(atom.getRegisterType()), 1);
    QFont font;
    font.setPixelSize(100);
    QFontMetrics fm(font);
    QString testText = text;
    if (text.size() == 1)
        testText = "XX";
    int refWidth = fm.horizontalAdvance(testText);
    float scale = r.width() / refWidth;
    font.setPixelSize(100 * scale);

    if (labelNeedsBackground(atom.getRegisterType(), atom.getNumber())) {
        painter->save();
        painter->setBrush(COLOR(RACV_COLOR_LABEL_BG));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(r);
        painter->restore();
    }

    // font.setPixelSize(RACV_PIXEL_PER_HP * fontSizes[text.size()] * r.width() / 130 );
    painter->setFont(font);

    painter->drawText(r, text, Qt::AlignCenter | Qt::AlignVCenter);
}
bool Module::haveRegister(AtomRegister atom)
{
    unsigned count = numRegisters(atom.getRegisterType());
    unsigned offset = numberOffset(atom.getRegisterType());
    bool have = atom.number() >= 1 + offset && atom.number() <= count + offset;
    return have;
}
void Module::hiliteRegisters(int usage, QChar type, unsigned number)
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
QRectF Module::registerRect(QChar type, unsigned number, int usage) const
{
    QPointF posHP = registerPosition(type, number); // in HP, mid
    float size = registerSize(type, number) * RACV_PIXEL_PER_HP;
    if (usage == 1)
        size = size * 2 / 3;
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
            QRectF r = registerRect(type, j+1, 1);
            if (r.contains(pos)) {
                return registerAtom(type, j+1).clone();
            }
        }
    }
    return 0;
}
AtomRegister Module::registerAtom(QChar type, unsigned number) const
{
    return AtomRegister(type, controllerNumber(), number + numberOffset(type));
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
