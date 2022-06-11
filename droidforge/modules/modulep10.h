#ifndef MODULEP10_H
#define MODULEP10_H

#include "module.h"

class ModuleP10 : public Module
{
public:
    ModuleP10() : Module("faceplate-p10") {}
    QString name() const { return "p10"; }
    QString faceplate() const { return "faceplate-p10";  }
    QString title() const { return "P10 Controller"; }
    float hp() const { return 5; }
    unsigned numRegisters(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
    bool labelNeedsBackground(QChar, unsigned n) const  { return true; };
};

#endif // MODULEP10_H
