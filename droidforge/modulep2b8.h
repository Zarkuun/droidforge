#ifndef MODULEP2B8_H
#define MODULEP2B8_H

#include "module.h"

class ModuleP2B8 : public Module
{
public:
    ModuleP2B8() {}
    QString name() const { return "p2b8"; }
    QString faceplate() const { return "faceplate-p2b8-off.jpg"; }
    QString title() const { return "P2B8 Controller"; }
    unsigned hp() const  { return 5; }
};

#endif // MODULEP2B8_H
