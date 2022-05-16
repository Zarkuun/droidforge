#ifndef MODULEG8_H
#define MODULEG8_H

#include "module.h"

class ModuleG8 : public Module
{
public:
    ModuleG8() {}
    QString name()  { return "g8"; }
    QString faceplate() { return "faceplate-g8-off"; }
    QString title() { return "G8 Gates Expander"; }
    unsigned hp() { return 4; }
};

#endif // MODULEG8_H
