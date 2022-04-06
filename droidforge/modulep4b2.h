#ifndef MODULEP4B2_H
#define MODULEP4B2_H

#include "module.h"

class ModuleP4B2 : public Module
{
public:
    ModuleP4B2();

    // Module interface
public:
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULEP4B2_H
