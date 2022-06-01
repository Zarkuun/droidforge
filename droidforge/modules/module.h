#ifndef MODULE_H
#define MODULE_H

#include "atomregister.h"
#include "registerlist.h"

#include <QString>
#include <QPixmap>
#include <QGraphicsItem>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Droid { class ModuleType; }
QT_END_NAMESPACE

#define MAX_CONTROLS_OF_TYPE 32

// for data(...) of QGraphicsItem
#define DATA_INDEX_CONTROLLER_INDEX 0 // starting from 0, not from 1!
#define DATA_INDEX_MODULE_NAME      1 // QString, etc. "b32"


class Module : public QGraphicsItem
{
    QPixmap faceplateImage;
    bool registerIsHilited[NUM_REGISTER_TYPES][MAX_CONTROLS_OF_TYPE];

public:
    Module(const QString &faceplate);
    virtual ~Module();
    virtual QString name() const = 0;
    virtual QString title() const = 0;
    virtual float hp() const = 0;
    virtual unsigned numRegisters(QChar) const { return 0; };
    virtual unsigned numberOffset(QChar) const { return 0; };
    virtual QPointF registerPosition(QChar, unsigned) const = 0; // in HP
    virtual float registerSize(QChar, unsigned) const = 0; // in HP

    bool isController() const;
    void clearHilites();
    void hiliteRegisters(bool on=true, QChar type='\0', unsigned number=0);
    const QPixmap *getFaceplateImage() const { return &faceplateImage; };
    AtomRegister *registerAt(const QPoint &pos) const;
    AtomRegister registerAtom(QChar type, unsigned number) const;
    void collectAllRegisters(RegisterList &rl, int number=-1) const;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

private:
    void paintHiliteRegister(QPainter *painter, QChar type, unsigned number);
    QRectF registerRect(QChar type, unsigned number) const;
};

#endif // MODULE_H
