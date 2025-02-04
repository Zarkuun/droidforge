#ifndef MODULEP8S8_H
#define MODULEP8S8_H

#include "module.h"

class ModuleP8S8 : public Module
{
public:
    ModuleP8S8(MainWindow *mainWindow) : Module(mainWindow, "p8s8") { }
    QString title() const { return "P8S8 Controller"; }
    float hp() const { return 8; }
    unsigned numRegisters(register_type_t type) const;
    bool labelNeedsBackground(register_type_t, unsigned) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    QPointF labelPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const;
    float labelWidth(register_type_t, unsigned) const;
    float rectAspect(register_type_t, unsigned) const;
};

#endif // MODULEP8S8_H
