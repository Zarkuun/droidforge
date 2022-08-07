#include "modulebuilder.h"

#include "modulemaster.h"
#include "moduleg8.h"
#include "modulex7.h"
#include "modulep2b8.h"
#include "modulep4b2.h"
#include "moduleb32.h"
#include "modulep10.h"
#include "modules10.h"
#include "modulem4.h"
#include "modulebling.h"

#include <QStringList>

const QStringList &ModuleBuilder::allControllers()
{
    static const QStringList controllers{"p2b8", "p4b2", "b32", "p10", "s10", "m4"};
    return controllers;
}
Module *ModuleBuilder::buildModule(QString name, const RegisterLabels *labels)
{
    Module *module;
    if (name == "master")
        module = new ModuleMaster();
    else if (name == "g8")
        module = new ModuleG8();
    else if (name == "x7")
        module = new ModuleX7();
    else if (name == "p4b2")
        module = new ModuleP4B2();
    else if (name == "p2b8")
        module = new ModuleP2B8();
    else if (name == "p10")
        module = new ModuleP10();
    else if (name == "s10")
        module = new ModuleS10();
    else if (name == "m4")
        module = new ModuleM4();
    else if (name == "b32")
        module = new ModuleB32();
    else
        module = new ModuleBling();
    if (labels)
        module->setLabels(labels);
    return module;
}
bool ModuleBuilder::controllerExists(QString name)
{
    return allControllers().contains(name);
}
void ModuleBuilder::allRegistersOf(QString name, unsigned number, RegisterList &rl)
{
    Module *m = buildModule(name);
    m->collectAllRegisters(rl, number);
    delete m;
}
