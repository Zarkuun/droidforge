#ifndef MODULETYPEINVALID_H
#define MODULETYPEINVALID_H

#include "moduletype.h"

class ModuleTypeInvalid : public ModuleType
{
public:
    ModuleTypeInvalid();
    QString name() { return "invalid"; };
    QString faceplate() { return "faceplate-blind-front.jpg"; };
    QString title() { return "Invalid"; };
    unsigned hp() { return 1; };
};

#endif // MODULETYPEINVALID_H
