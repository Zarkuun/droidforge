#ifndef MODULEM4_H
#define MODULEM4_H

#include "module.h"

class ModuleM4 : public Module
{
public:
    ModuleM4() : Module("faceplate-m4") {  }
    QString name() const { return "m4"; }
    QString faceplate() const { return "faceplate-m4";  }
    QString title() const { return "M4 Motor Fader Unit";  }
    float hp() const { return 14; }
    unsigned numControls(QChar type) const;

protected:
    QPointF controlPosition(QChar type, unsigned number) const;
    float controlSize(QChar type, unsigned number) const;
};

#endif // MODULEM4_H
