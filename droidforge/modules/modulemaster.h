#ifndef MODULEMASTER_H
#define MODULEMASTER_H

#include "module.h"

class ModuleMaster : public Module
{
public:
    ModuleMaster(MainWindow *mainWindow) : Module(mainWindow, "master") {  }
    QString title() const { return "DROID master"; }
    float hp() const { return 8; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const { return true; };
    float labelDistance(register_type_t, unsigned) const; // in HP
    float labelWidth(register_type_t, unsigned) const { return RACV_JACK_LABEL_WIDTH; };
    float rectAspect(register_type_t) const;
};



#endif // MODULEMASTER_H
