#ifndef MODULEP10_H
#define MODULEP10_H

#include "module.h"

class ModuleP10 : public Module
{
public:
    ModuleP10();

public:
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULEP10_H
