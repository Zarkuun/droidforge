#include "modulebuilder.h"

#include "modulemaster.h"
#include "modulemaster18.h"
#include "moduleg8.h"
#include "modulex7.h"
#include "modulep2b8.h"
#include "modulep4b2.h"
#include "moduleb32.h"
#include "modulep10.h"
#include "modules10.h"
#include "modulep8s8.h"
#include "modulee4.h"
#include "modulem4.h"
#include "modulebling.h"
#include "globals.h"

#include <QStringList>

const QStringList &ModuleBuilder::allControllers()
{
    static const QStringList controllers{"p2b8", "p4b2", "b32", "p10", "s10", "p8s8", "e4", "m4"};
    return controllers;
}
Module *ModuleBuilder::buildModule(QString name, const RegisterLabels *labels)
{
    Module *module;
    if (name == "master")
        module = new ModuleMaster(mainWindow);
    else if (name == "master18")
        module = new ModuleMaster18(mainWindow);
    else if (name == "g8")
        module = new ModuleG8(mainWindow);
    else if (name == "x7")
        module = new ModuleX7(mainWindow);
    else if (name == "p4b2")
        module = new ModuleP4B2(mainWindow);
    else if (name == "p2b8")
        module = new ModuleP2B8(mainWindow);
    else if (name == "p10")
        module = new ModuleP10(mainWindow);
    else if (name == "s10")
        module = new ModuleS10(mainWindow);
    else if (name == "p8s8")
        module = new ModuleP8S8(mainWindow);
    else if (name == "e4")
        module = new ModuleE4(mainWindow);
    else if (name == "m4")
        module = new ModuleM4(mainWindow);
    else if (name == "b32")
        module = new ModuleB32(mainWindow);
    else
        module = new ModuleBling(mainWindow);
    if (labels)
        module->setLabels(labels);
    return module;
}
bool ModuleBuilder::controllerExists(QString name)
{
    return allControllers().contains(name);
}

void ModuleBuilder::allRegistersOf(QString name, unsigned controller, unsigned g8, RegisterList &rl)
{
    ModuleBuilder mb(0);
    Module *m = mb.buildModule(name);
    if (controller)
        m->setData(DATA_INDEX_CONTROLLER_INDEX, controller - 1);
    else if (g8)
        m->setData(DATA_INDEX_G8_NUMBER, g8);

    m->collectAllRegisters(rl);
    delete m;
}
