#ifndef RACK_H
#define RACK_H

#include <QList>
#include "module.h"

class Rack
{
    QList<Module *> modules;

public:
    Rack();
    ~Rack();
    QListIterator<Module *> iterator() { return QListIterator<Module *>(modules); };

    void addModule(Module *module); // take over ownership
};

#endif // RACK_H
