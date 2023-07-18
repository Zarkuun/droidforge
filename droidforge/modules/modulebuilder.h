#ifndef MODULEBUILDER_H
#define MODULEBUILDER_H

#include "module.h"
#include "atomregister.h"
#include "registerlabels.h"

class MainWindow;

class ModuleBuilder
{
    MainWindow *mainWindow;

public:
    ModuleBuilder(MainWindow *mainWindow) : mainWindow(mainWindow) {};
    static const QStringList &allControllers();
    Module *buildModule(QString name, const RegisterLabels *labels = 0);
    static bool controllerExists(QString name);
    static void allRegistersOf(QString name, unsigned controller, unsigned g8, RegisterList &rl);
};

#endif // MODULEBUILDER_H
