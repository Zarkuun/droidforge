#ifndef MODULEP10_H
#define MODULEP10_H

#include "module.h"

class ModuleP10 : public Module
{
public:
    ModuleP10() {}
    QString name() const { return "p10"; }
    QString faceplate() const { return "faceplate-p10.jpg";  }
    QString title() const { return "P10 Controller"; }
    unsigned hp() const { return 5; }
};

#endif // MODULEP10_H
