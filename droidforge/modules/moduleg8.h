#ifndef MODULEG8_H
#define MODULEG8_H

#include "module.h"

class ModuleG8 : public Module
{
public:
    ModuleG8(MainWindow *mainWindow) : Module(mainWindow, "g8") {}
    QString title() const { return "G8 Gates Expander"; }
    float hp() const { return 4; }
    unsigned numRegisters(register_type_t type) const;
    unsigned numberOffset(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const { return true; };
    float labelDistance(register_type_t, unsigned) const; // in HP
    float labelWidth(register_type_t, unsigned) const { return RACV_JACK_LABEL_WIDTH; };
};

#endif // MODULEG8_H
