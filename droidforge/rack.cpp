#include "rack.h"

Rack::Rack()
{

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
