#include "modulebuilder.h"

#include "moduletypemaster.h"
#include "moduletypeg8.h"
#include "moduletypeb32.h"
#include "moduletypeinvalid.h"

ModuleBuilder::ModuleBuilder()
{

}

ModuleType *ModuleBuilder::buildModule(QString name)
{
    if (name == "master")
        return new ModuleTypeMaster();
    else if (name == "g8")
        return new ModuleTypeG8();
    else if (name == "b32")
        return new ModuleTypeB32();
    else
        return new ModuleTypeInvalid();

}
