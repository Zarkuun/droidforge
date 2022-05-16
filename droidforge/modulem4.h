#ifndef MODULEM4_H
#define MODULEM4_H

#include "module.h"

class ModuleM4 : public Module
{
public:
    ModuleM4() {  }
    QString name() const { return "m4"; }
    QString faceplate() const { return "faceplate-m4.jpg";  }
    QString title() const { return "M4 Motor Fader Unit";  }
    unsigned hp() const { return 14; }
};

#endif // MODULEM4_H
