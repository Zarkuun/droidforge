#ifndef MODULEB32_H
#define MODULEB32_H

#include "module.h"
#include "tuning.h"

class ModuleB32 : public Module
{
public:
    ModuleB32(MainWindow *mainWindow) : Module(mainWindow, "b32") {}
    QString title() const { return "B32 Controller"; };
    float hp() const { return 10; };
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    float labelDistance(register_type_t, unsigned) const;
    float labelWidth(register_type_t, unsigned) const { return RACV_BUTTON_LABEL_WIDTH; };
};

#endif // MODULEB32_H
