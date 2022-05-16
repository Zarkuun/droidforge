#ifndef MODULEX7_H
#define MODULEX7_H

#include "module.h"

class ModuleX7 : public Module
{
public:
    ModuleX7() {}
    QString name() { return "x7"; }
    QString faceplate() { return "faceplate-x7-off"; }
    QString title() { return "X7 MIDI/USB Expander"; }
    unsigned hp() { return 4; }
};

#endif // MODULEX7_H
