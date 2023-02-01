#ifndef MODULEBLIND_H
#define MODULEBLIND_H

#include "module.h"

class ModuleBlind : public Module
{
public:
    ModuleBlind() : Module("faceplate-blind-front") {  }
    QString faceplate() const { return "faceplate-blind-front"; }
    QString title() const { return "1HP Blind panel"; }
    float hp() const { return 1; }
};

#endif // MODULEBLIND_H
