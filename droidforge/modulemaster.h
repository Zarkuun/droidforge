#ifndef MODULEMASTER_H
#define MODULEMASTER_H

#include "module.h"

class ModuleMaster : public Module
{
public:
    ModuleMaster();
    QString name();
    QString faceplate();
    QString title();
    unsigned hp();
};

#endif // MODULEMASTER_H
