#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QPixmap>
#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
namespace Droid { class ModuleType; }
QT_END_NAMESPACE

class Module : public QGraphicsItem
{
    QPixmap faceplateImage;

public:
    Module(const QString &faceplate);
    virtual ~Module();
    virtual QString name() const = 0;
    virtual QString title() const = 0;
    virtual unsigned hp() const = 0;
    const QPixmap *getFaceplateImage() const { return &faceplateImage; };

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
};




#endif // MODULE_H
