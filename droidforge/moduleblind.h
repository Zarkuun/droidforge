#ifndef MODULEBLIND_H
#define MODULEBLIND_H

#include "module.h"

class ModuleBlind : public Module
{
public:
    ModuleBlind() : Module("faceplate-blind-front") {  }
    QString name() const { return "blind"; }
    QString faceplate() const { return "faceplate-blind-front"; }
    QString title() const { return "1HP Blind panel"; }
    unsigned hp() const { return 1; }
};

#endif // MODULEBLIND_H
