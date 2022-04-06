#ifndef MODULEM4_H
#define MODULEM4_H

#include "module.h"

class ModuleM4 : public Module
{
public:
    ModuleM4();

public:
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULEM4_H
