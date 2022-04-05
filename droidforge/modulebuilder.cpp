#include "modulebuilder.h"

#include "modulemaster.h"
#include "moduleg8.h"
#include "modulep2b8.h"
#include "moduleb32.h"
#include "moduleinvalid.h"

ModuleBuilder::ModuleBuilder()
{

}

Module *ModuleBuilder::buildModule(QString name)
{
    if (name == "master")
        return new ModuleMaster();
    else if (name == "g8")
        return new ModuleG8();
    else if (name == "p2b8")
        return new ModuleP2B8();
    else if (name == "b32")
        return new ModuleB32();
    else
        return new ModuleInvalid();

}
