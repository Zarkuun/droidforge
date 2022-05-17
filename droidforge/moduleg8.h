#ifndef MODULEG8_H
#define MODULEG8_H

#include "module.h"

class ModuleG8 : public Module
{
public:
    ModuleG8() : Module("faceplate-g8-off") {}
    QString name() const { return "g8"; }
    QString faceplate() const { return "faceplate-g8-off"; }
    QString title() const { return "G8 Gates Expander"; }
    unsigned hp() const { return 4; }
};

#endif // MODULEG8_H
