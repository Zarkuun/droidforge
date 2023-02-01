#ifndef MODULES10_H
#define MODULES10_H

#include "module.h"

class ModuleS10 : public Module
{
public:
    ModuleS10() : Module("s10") {  }
    QString title() const  { return "S10 Controller";  }
    float hp() const  { return 5; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const { return true; };
    float labelDistance(register_type_t, unsigned) const;
    float labelWidth(register_type_t, unsigned) const;
};

#endif // MODULES10_H
