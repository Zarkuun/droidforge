#ifndef MODULEX7_H
#define MODULEX7_H

#include "module.h"

class ModuleX7 : public Module
{
public:
    ModuleX7() : Module("faceplate-x7-off") {};
    QString name() const { return "x7"; }
    QString faceplate() const { return "faceplate-x7-off"; }
    QString title() const { return "X7 MIDI/USB Expander"; }
    unsigned hp() const { return 4; }
};

#endif // MODULEX7_H
