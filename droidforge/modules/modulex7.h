#ifndef MODULEX7_H
#define MODULEX7_H

#include "module.h"

class ModuleX7 : public Module
{
public:
    ModuleX7() : Module("x7") {};
    QString title() const { return "X7 MIDI/USB Expander"; }
    float hp() const { return 4; }
    unsigned numRegisters(QChar type) const;
    unsigned numberOffset(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
    bool labelNeedsBackground(QChar, unsigned) const { return true; };
};

#endif // MODULEX7_H
