#ifndef MODULEE4_H
#define MODULEE4_H

#include "module.h"

class ModuleE4 : public Module
{
public:
    ModuleE4(MainWindow *mainWindow) : Module(mainWindow, "e4") {  }
    QString title() const { return "E4 Encoder Controller";  }
    float hp() const { return 6; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const; // in HP
    float labelWidth(register_type_t, unsigned) const;
    float rectAspect(register_type_t) const;
};

#endif // MODULEE4_H
