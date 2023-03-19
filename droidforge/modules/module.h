#ifndef MODULE_H
#define MODULE_H

#include "atomregister.h"
#include "registerlabels.h"
#include "registerlist.h"
#include "mousedragger.h"
#include "tuning.h"

#include <QString>
#include <QPixmap>
#include <QImage>
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


class MainWindow;

class Module : public QGraphicsItem
{
    MainWindow *mainWindow;
    QString name;
    QImage faceplateImage;
    int registerHilite[NUM_REGISTER_TYPES][MAX_CONTROLS_OF_TYPE]; // 0: off, 1: used, 2: current
    const RegisterLabels *registerLabels; // points into current patch
    unsigned pixelHeight;

public:
    Module(MainWindow *mainWindow, const QString &name);
    void setPixelHeight(unsigned height) { pixelHeight = height; };
    void setLabels(const RegisterLabels *labels) { registerLabels = labels; };
    virtual ~Module();
    QString getName() const { return name; };
    virtual QString title() const = 0;
    virtual float hp() const = 0;
    virtual unsigned numRegisters(register_type_t) const { return 0; };
    virtual unsigned numberOffset(register_type_t) const { return 0; };
    virtual QPointF registerPosition(register_type_t, unsigned) const = 0; // in HP
    virtual float registerSize(register_type_t, unsigned) const = 0; // in HP
    virtual float labelDistance(register_type_t, unsigned) const { return 0; }; // in HP
    virtual float labelWidth(register_type_t, unsigned) const { return 2.0; }; // never called
    virtual bool labelNeedsBackground(register_type_t, unsigned) const { return false; };
    QRectF boundingRect() const override;
    QRectF moduleRect() const;

    bool isController() const;
    void clearHilites();
    void hiliteRegisters(int usage, register_type_t type=REGISTER_TYPE_NULL, unsigned number=0);
    const QImage *getFaceplateImage() const { return &faceplateImage; };
    // const QPixmap *getFaceplatePixmap() const { return &faceplatePixmap; };
    AtomRegister *registerAt(const QPoint &pos) const;
    AtomRegister registerAtom(register_type_t type, unsigned number) const;
    void collectAllRegisters(RegisterList &rl, int number=-1) const;
    unsigned controllerNumber() const;
    void createRegisterItems(QGraphicsScene *scene, int moduleIndex, int controllerIndex);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    virtual float rectAspect(register_type_t) const { return 0.0; };

private:
    void paintHiliteRegister(QPainter *painter, int usage, register_type_t type, unsigned number);
    QRectF registerRect(register_type_t type, unsigned number, float aspect, int usage) const;
    void paintRegisterLabels(QPainter *painter);
    void paintRegisterHilites(QPainter *painter);
    bool haveRegister(AtomRegister atom);
    void paintRegisterLabel(QPainter *painter, AtomRegister atom, const RegisterLabel &label);
    void paintRegisterHilites(QPainter *painter, int usage);
};

#endif // MODULE_H
