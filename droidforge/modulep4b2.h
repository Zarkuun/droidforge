#ifndef MODULEP4B2_H
#define MODULEP4B2_H

#include "module.h"

class ModuleP4B2 : public Module
{
public:
    ModuleP4B2() : Module("p4b2") { }
    QString title() const { return "P4B2 Controller"; }
    float hp() const { return 5; }
    unsigned numRegisters(register_type_t type) const;
    bool labelNeedsBackground(register_type_t, unsigned) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const;
    float labelWidth(register_type_t, unsigned) const;
};

#endif // MODULEP4B2_H
