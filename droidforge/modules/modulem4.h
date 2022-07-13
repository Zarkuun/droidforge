#ifndef MODULEM4_H
#define MODULEM4_H

#include "module.h"

class ModuleM4 : public Module
{
public:
    ModuleM4() : Module("m4") {  }
    QString name() const { return "m4"; }
    QString title() const { return "M4 Motor Fader Unit";  }
    float hp() const { return 14; }
    unsigned numRegisters(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
};

#endif // MODULEM4_H
