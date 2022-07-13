#ifndef MODULEG8_H
#define MODULEG8_H

#include "module.h"

class ModuleG8 : public Module
{
public:
    ModuleG8() : Module("g8") {}
    QString name() const { return "g8"; }
    QString title() const { return "G8 Gates Expander"; }
    float hp() const { return 4; }
    unsigned numRegisters(QChar type) const;
    unsigned numberOffset(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
    bool labelNeedsBackground(QChar, unsigned) const { return true; };
};

#endif // MODULEG8_H
