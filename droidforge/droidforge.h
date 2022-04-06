#ifndef DROIDFORGE_H
#define DROIDFORGE_H

#include "droidfirmware.h"

class DroidForge
{
    DroidFirmware firmware;

public:
    DroidForge();
};

extern DroidForge *the_forge;
extern DroidFirmware *the_firmware;


#endif // DROIDFORGE_H
