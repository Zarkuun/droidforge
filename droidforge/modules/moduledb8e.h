#ifndef MODULEDB8E_H
#define MODULEDB8E_H

#include "module.h"

class ModuleDB8E : public Module
{
public:
    ModuleDB8E(MainWindow *mainWindow) : Module(mainWindow, "db8e") {  }
    QString title() const { return "DB8E Display Controller";  }
    float hp() const { return 6; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const; // in HP
    float labelWidth(register_type_t, unsigned) const;
    float rectAspect(register_type_t, unsigned) const;
};

#endif // MODULEDB8E_H
