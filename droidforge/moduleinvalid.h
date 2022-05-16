#ifndef MODULEINVALID_H
#define MODULEINVALID_H

#include "module.h"

class ModuleInvalid : public Module
{
public:
    ModuleInvalid() {}
    QString name() const { return "invalid"; };
    QString faceplate() const { return "faceplate-blind-front.jpg"; };
    QString title() const { return "Invalid"; };
    unsigned hp() const { return 1; };
};

#endif // MODULEINVALID_H
