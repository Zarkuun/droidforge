#ifndef MODULEBLING_H
#define MODULEBLING_H

#include "module.h"

class ModuleBling : public Module
{
public:
    ModuleBling(MainWindow *mainWindow) : Module(mainWindow, "bling") {}
    QString faceplate() const { return "bling"; };
    QString title() const { return "1 HP blind plate"; };
    float hp() const { return 1; };
    QPointF registerPosition(register_type_t, unsigned) const { return QPointF(0, 0); };
    float registerSize(register_type_t, unsigned) const { return 0.0; };
};

#endif // MODULEBLING_H
