#ifndef MODULEINVALID_H
#define MODULEINVALID_H

#include "module.h"

class ModuleInvalid : public Module
{
public:
    ModuleInvalid() : Module("faceplate-blind-front") {}
    QString name() const { return "invalid"; };
    QString faceplate() const { return "faceplate-blind-front"; };
    QString title() const { return "Invalid"; };
    float hp() const { return 1; };
};

#endif // MODULEINVALID_H
