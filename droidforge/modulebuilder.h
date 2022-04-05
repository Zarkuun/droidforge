#ifndef MODULEBUILDER_H
#define MODULEBUILDER_H

#include "moduletype.h"

class ModuleBuilder
{
public:
    ModuleBuilder();
    static ModuleType *buildModule(QString name);
};

#endif // MODULEBUILDER_H
