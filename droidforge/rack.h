#ifndef RACK_H
#define RACK_H

#include <QList>

#include "module.h"
#include "patch.h"

class Rack
{
    QList<Module *> modules;

public:
    Rack();
    Rack(const Patch &patch);
    ~Rack();
    QListIterator<Module *> iterator() { return QListIterator<Module *>(modules); };

    void addModule(Module *module); // take over ownership
};

#endif // RACK_H
