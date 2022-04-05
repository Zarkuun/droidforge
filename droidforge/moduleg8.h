#ifndef MODULEG8_H
#define MODULEG8_H

#include "module.h"

class ModuleG8 : public Module
{
public:
    ModuleG8();
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULEG8_H
