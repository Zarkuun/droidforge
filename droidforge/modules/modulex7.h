#ifndef MODULEX7_H
#define MODULEX7_H

#include "module.h"

// TODO: Schwierigkeit, weil sowohl R als auch G
// nicht bei 1 anfangen

class ModuleX7 : public Module
{
public:
    ModuleX7() : Module("faceplate-x7-off") {};
    QString name() const { return "x7"; }
    QString faceplate() const { return "faceplate-x7-off"; }
    QString title() const { return "X7 MIDI/USB Expander"; }
    float hp() const { return 4; }
    unsigned numRegisters(QChar type) const;
    unsigned numberOffset(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
    bool labelNeedsBackground(QChar, unsigned n) const { return true; };
};

#endif // MODULEX7_H
