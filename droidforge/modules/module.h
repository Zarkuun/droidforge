#ifndef MODULE_H
#define MODULE_H

#include "atomregister.h"
#include "registerlabels.h"
#include "registerlist.h"
#include "mousedragger.h"

#include <QString>
#include <QPixmap>
#include <QGraphicsItem>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Droid { class ModuleType; }
QT_END_NAMESPACE

#define MAX_CONTROLS_OF_TYPE 32

// for data(...) of QGraphicsItem
#define DATA_INDEX_MODULE_INDEX     0
#define DATA_INDEX_CONTROLLER_INDEX 1 // starting from 0, not from 1!
#define DATA_INDEX_MODULE_NAME      2 // QString, e.g. "b32"
#define DATA_INDEX_REGISTER_NAME    3 // QString, e.g. "L1.4"
#define DATA_INDEX_REGISTER         4


class Module : public QGraphicsItem
{
    QString name;
    QPixmap faceplateImage;
    int registerHilite[NUM_REGISTER_TYPES][MAX_CONTROLS_OF_TYPE]; // 0: off, 1: used, 2: current
    const RegisterLabels *registerLabels; // points into current patch

public:
    Module(const QString &name);
    void setLabels(const RegisterLabels *labels) { registerLabels = labels; };
    virtual ~Module();
    QString getName() const { return name; };
    virtual QString title() const = 0;
    virtual float hp() const = 0;
    virtual unsigned numRegisters(register_type_t) const { return 0; };
    virtual unsigned numberOffset(register_type_t) const { return 0; };
    virtual QPointF registerPosition(register_type_t, unsigned) const = 0; // in HP
    virtual float registerSize(register_type_t, unsigned) const = 0; // in HP
    virtual bool labelNeedsBackground(register_type_t, unsigned) const { return false; };
    QRectF boundingRect() const override;
    QRectF moduleRect() const;

    bool isController() const;
    void clearHilites();
    void hiliteRegisters(int usage, register_type_t type=REGISTER_TYPE_NULL, unsigned number=0);
    const QPixmap *getFaceplateImage() const { return &faceplateImage; };
    AtomRegister *registerAt(const QPoint &pos) const;
    AtomRegister registerAtom(register_type_t type, unsigned number) const;
    void collectAllRegisters(RegisterList &rl, int number=-1) const;
    unsigned controllerNumber() const;
    void createRegisterItems(QGraphicsScene *scene, int moduleIndex, int controllerIndex);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

private:
    void paintHiliteRegister(QPainter *painter, int usage, register_type_t type, unsigned number);
    QRectF registerRect(register_type_t type, unsigned number, int usage) const;
    void paintRegisterLabels(QPainter *painter);
    void paintRegisterHilites(QPainter *painter);
    bool haveRegister(AtomRegister atom);
    void paintRegisterLabel(QPainter *painter, AtomRegister atom, const RegisterLabel &label);
};

#endif // MODULE_H
