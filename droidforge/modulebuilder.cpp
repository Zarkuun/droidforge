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
#include "moduleinvalid.h"

#include <QStringList>

ModuleBuilder::ModuleBuilder()
{
}


Module *ModuleBuilder::buildModule(QString name)
{
    if (name == "master")
        return new ModuleMaster();
    else if (name == "g8")
        return new ModuleG8();
    else if (name == "x7")
        return new ModuleX7();
    else if (name == "p4b2")
        return new ModuleP4B2();
    else if (name == "p2b8")
        return new ModuleP2B8();
    else if (name == "p10")
        return new ModuleP10();
    else if (name == "s10")
        return new ModuleS10();
    else if (name == "m4")
        return new ModuleM4();
    else if (name == "b32")
        return new ModuleB32();
    else
        return new ModuleInvalid();

}


bool ModuleBuilder::controllerExists(QString name)
{
    static QStringList controllers {
        "p2b8", "p4b2", "b32", "p10", "s10", "m4",
    };

    return controllers.contains(name);
}
