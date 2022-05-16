#ifndef MODULEMASTER_H
#define MODULEMASTER_H

#include "module.h"

class ModuleMaster : public Module
{
public:
    ModuleMaster() {  }
    QString name() const { return "master"; }
    QString faceplate() const { return "faceplate-master-off";  }
    QString title() const { return "DROID master"; }
    unsigned hp() const { return 8; }
};

#endif // MODULEMASTER_H
