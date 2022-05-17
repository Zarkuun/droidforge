#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QPixmap>
#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
namespace Droid { class ModuleType; }
QT_END_NAMESPACE

#define NUM_CONTROL_TYPES 10
#define MAX_CONTROLS_OF_TYPE 32
const char register_types[NUM_CONTROL_TYPES] = { 'B', 'L', 'P', 'S', 'I', 'O', 'G', 'N', 'R', 'X' };

class Module : public QGraphicsItem
{
    QPixmap faceplateImage;
    bool controlHilit[NUM_CONTROL_TYPES][MAX_CONTROLS_OF_TYPE];

public:
    Module(const QString &faceplate);
    virtual ~Module();
    virtual QString name() const = 0;
    virtual QString title() const = 0;
    virtual float hp() const = 0;
    virtual unsigned numControls(QChar) const { return 0; };

    void hiliteControls(bool on=true, QChar type='\0', unsigned number=0);
    const QPixmap *getFaceplateImage() const { return &faceplateImage; };

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    virtual QPointF controlPosition(QChar, unsigned) { return QPointF(0, 0); }; // in HP
    virtual float controlSize(QChar, unsigned) { return 2; }; // in HP

private:
    void paintHiliteControl(QPainter *painter, QChar type, unsigned number);
};




#endif // MODULE_H
