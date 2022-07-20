#ifndef MODULEMASTER_H
#define MODULEMASTER_H

#include "module.h"

class ModuleMaster : public Module
{
public:
    ModuleMaster() : Module("master") {  }
    QString title() const { return "DROID master"; }
    float hp() const { return 8; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const { return true; };
};

#endif // MODULEMASTER_H
