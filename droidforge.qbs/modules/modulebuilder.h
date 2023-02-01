#ifndef MODULEBUILDER_H
#define MODULEBUILDER_H

#include "module.h"
#include "atomregister.h"
#include "registerlabels.h"

class ModuleBuilder
{
public:
    static const QStringList &allControllers();
    static Module *buildModule(QString name, const RegisterLabels *labels = 0);
    static bool controllerExists(QString name);
    static void allRegistersOf(QString name, unsigned number, RegisterList &rl);
};

#endif // MODULEBUILDER_H
