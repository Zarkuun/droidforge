#ifndef MODULETYPEB32_H
#define MODULETYPEB32_H

#include "moduletype.h"

class ModuleTypeB32 : public ModuleType
{
public:
    ModuleTypeB32();
    QString name() { return "b32"; };
    QString faceplate() { return "faceplate-b32-off"; };
    QString title() { return "B32 Controller"; };
    unsigned hp() { return 10; };
};

#endif // MODULETYPEB32_H
