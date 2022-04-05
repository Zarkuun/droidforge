#ifndef MODULEBUILDER_H
#define MODULEBUILDER_H

#include "module.h"

class ModuleBuilder
{
public:
    ModuleBuilder();
    static Module *buildModule(QString name);
};

#endif // MODULEBUILDER_H
