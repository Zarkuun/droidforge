#ifndef MODULES10_H
#define MODULES10_H

#include "module.h"

class ModuleS10 : public Module
{
public:
    ModuleS10() {  }
    QString name() const { return "s10";  }
    QString faceplate() const  { return "faceplate-s10.jpg";  }
    QString title() const  { return "S10 Controller";  }
    unsigned hp() const  { return 5; }
};

#endif // MODULES10_H
