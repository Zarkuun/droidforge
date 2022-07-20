#ifndef MODULEP10_H
#define MODULEP10_H

#include "module.h"

class ModuleP10 : public Module
{
public:
    ModuleP10() : Module("p10") {}
    QString title() const { return "P10 Controller"; }
    float hp() const { return 5; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const  { return true; };
};

#endif // MODULEP10_H
