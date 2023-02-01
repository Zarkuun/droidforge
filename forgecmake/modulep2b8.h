#ifndef MODULEP2B8_H
#define MODULEP2B8_H

#include "module.h"

class ModuleP2B8 : public Module
{
public:
    ModuleP2B8() : Module("p2b8") {}
    QString title() const { return "P2B8 Controller"; }
    float hp() const  { return 5; }
    unsigned numRegisters(register_type_t type) const;
    bool labelNeedsBackground(register_type_t, unsigned) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const;
    float labelWidth(register_type_t, unsigned) const;
};

#endif // MODULEP2B8_H
