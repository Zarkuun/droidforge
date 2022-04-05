#ifndef MODULETYPEG8_H
#define MODULETYPEG8_H

#include "moduletype.h"

class ModuleTypeG8 : public ModuleType
{
public:
    ModuleTypeG8();
    QString name()  { return "g8"; };
    QString faceplate() {return "faceplate-g8-off"; };
    QString title() { return "G8 Gates Expander"; };
    unsigned hp() { return 4; };
};

#endif // MODULETYPEG8_H
