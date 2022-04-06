#ifndef MODULE_H
#define MODULE_H

#include <QString>

QT_BEGIN_NAMESPACE
namespace Droid { class ModuleType; }
QT_END_NAMESPACE

class Module
{
public:
    virtual ~Module() {};
    virtual QString name() = 0;
    virtual QString faceplate() = 0;
    virtual QString title() = 0;
    virtual unsigned hp() = 0;
};


#endif // MODULE_H
