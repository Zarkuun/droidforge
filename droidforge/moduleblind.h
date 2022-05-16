#ifndef MODULEBLIND_H
#define MODULEBLIND_H

#include "module.h"

class ModuleBlind : public Module
{
public:
    ModuleBlind();
    QString name()  { return "blind"; }
    QString faceplate() { return "faceplate-blind-front"; }
    QString title() { return "1HP Blind panel"; }
    unsigned hp() { return 1; }
};

#endif // MODULEBLIND_H
