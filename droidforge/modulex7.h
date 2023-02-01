#ifndef MODULEX7_H
#define MODULEX7_H

#include "module.h"

class ModuleX7 : public Module
{
public:
    ModuleX7() : Module("x7") {};
    QString title() const { return "X7 MIDI/USB Expander"; }
    float hp() const { return 4; }
    unsigned numRegisters(register_type_t type) const;
    unsigned numberOffset(register_type_t type) const;

protected:
    QPointF registerPosition(register_type_t type, unsigned number) const;
    float registerSize(register_type_t type, unsigned number) const;
    bool labelNeedsBackground(register_type_t, unsigned) const { return true; };
    float labelDistance(register_type_t, unsigned) const; // in HP
    float labelWidth(register_type_t, unsigned) const { return RACV_JACK_LABEL_WIDTH; };
};

#endif // MODULEX7_H
