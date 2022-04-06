#include "rack.h"
#include "modulebuilder.h"

Rack::Rack()
{

}

Rack::Rack(const Patch &patch)
{
    for (qsizetype i=0; i<patch.controllers.size(); i++)
        addModule(ModuleBuilder::buildModule(patch.controllers.at(i)));
}


Rack::~Rack()
{
    QListIterator<Module *> i(modules);
    while (i.hasNext())
        delete i.next();
}


void Rack::addModule(Module *module)
{
    modules.push_back(module);
}
