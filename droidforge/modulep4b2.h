#ifndef MODULEP4B2_H
#define MODULEP4B2_H

#include "module.h"

class ModuleP4B2 : public Module
{
public:
    ModuleP4B2() { }
    QString name() const { return "p4b2"; }
    QString faceplate() const { return "faceplate-p4b2-off.jpg"; }
    QString title() const { return "P4B2 Controller"; }
    unsigned hp() const { return 5; }
};

#endif // MODULEP4B2_H
