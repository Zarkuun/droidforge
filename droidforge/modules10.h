#ifndef MODULES10_H
#define MODULES10_H

#include "module.h"

class ModuleS10 : public Module
{
public:
    ModuleS10();

public:
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULES10_H
