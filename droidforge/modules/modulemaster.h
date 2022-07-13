#ifndef MODULEMASTER_H
#define MODULEMASTER_H

#include "module.h"

class ModuleMaster : public Module
{
public:
    ModuleMaster() : Module("master") {  }
    QString title() const { return "DROID master"; }
    float hp() const { return 8; }
    unsigned numRegisters(QChar type) const;

protected:
    QPointF registerPosition(QChar type, unsigned number) const;
    float registerSize(QChar type, unsigned number) const;
    bool labelNeedsBackground(QChar, unsigned) const { return true; };
};

#endif // MODULEMASTER_H
