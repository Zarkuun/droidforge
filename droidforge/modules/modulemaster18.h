#ifndef MODULEMASTER18_H
#define MODULEMASTER18_H

#include "module.h"

class ModuleMaster18 : public Module
{
public:
    ModuleMaster18(MainWindow *mainWindow) : Module(mainWindow, "master18") {  }
    QString title() const { return "MASTER18"; }
    float hp() const { return 6; }
    unsigned numRegisters(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const { return true; };
    float labelDistance(register_type_t, unsigned) const; // in HP
    float labelWidth(register_type_t, unsigned) const { return RACV_JACK_LABEL_WIDTH; };
};

#endif // MODULEMASTER18_H
