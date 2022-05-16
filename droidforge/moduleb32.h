#ifndef MODULEB32_H
#define MODULEB32_H

#include "module.h"

class ModuleB32 : public Module
{
public:
    ModuleB32() {}
    QString name() const { return "b32"; };
    QString faceplate() const { return "faceplate-b32-off"; };
    QString title() const { return "B32 Controller"; };
    unsigned hp() const { return 10; };
};

#endif // MODULEB32_H
