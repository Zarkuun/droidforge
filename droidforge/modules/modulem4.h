#ifndef MODULEM4_H
#define MODULEM4_H

#include "module.h"

class ModuleM4 : public Module
{
public:
    ModuleM4() : Module("m4") {  }
    QString title() const { return "M4 Motor Fader Unit";  }
    float hp() const { return 14; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const; // in HP
};

#endif // MODULEM4_H
