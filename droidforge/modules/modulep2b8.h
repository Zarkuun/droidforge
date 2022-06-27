#ifndef MODULEP2B8_H
#define MODULEP2B8_H

#include "module.h"

class ModuleP2B8 : public Module
{
public:
    ModuleP2B8() : Module("P2B8") {}
    QString name() const { return "p2b8"; }
    QString title() const { return "P2B8 Controller"; }
    float hp() const  { return 5; }
    unsigned numRegisters(QChar type) const;
    bool labelNeedsBackground(QChar, unsigned) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
};

#endif // MODULEP2B8_H
