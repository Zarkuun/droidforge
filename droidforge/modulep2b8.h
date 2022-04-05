#ifndef MODULEP2B8_H
#define MODULEP2B8_H

#include "module.h"

class ModuleP2B8 : public Module
{
public:
    ModuleP2B8();

    // Module interface
public:
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULEP2B8_H
