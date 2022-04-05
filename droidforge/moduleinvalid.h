#ifndef MODULEINVALID_H
#define MODULEINVALID_H

#include "module.h"

class ModuleInvalid : public Module
{
public:
    ModuleInvalid();
    QString name();;
    QString faceplate();;
    QString title();;
    unsigned hp();;
};

#endif // MODULEINVALID_H
