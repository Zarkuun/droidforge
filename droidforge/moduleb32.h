#ifndef MODULEB32_H
#define MODULEB32_H

#include "module.h"

class ModuleB32 : public Module
{
public:
    ModuleB32();
    QString name();;
    QString faceplate();;
    QString title();;
    unsigned hp();;
};

#endif // MODULEB32_H
