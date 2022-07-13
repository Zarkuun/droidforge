#ifndef MODULEB32_H
#define MODULEB32_H

#include "module.h"

class ModuleB32 : public Module
{
public:
    ModuleB32() : Module("b32") {}
    QString name() const { return "b32"; };
    QString title() const { return "B32 Controller"; };
    float hp() const { return 10; };
    unsigned numRegisters(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
};

#endif // MODULEB32_H
