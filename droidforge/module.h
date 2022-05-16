#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Droid { class ModuleType; }
QT_END_NAMESPACE

class Module
{
public:
    virtual ~Module() {};
    virtual QString name() const = 0;
    virtual QString faceplate() const = 0;
    virtual QString title() const = 0;
    virtual unsigned hp() const = 0;
    QPixmap *faceplateImage() const;

};




#endif // MODULE_H
