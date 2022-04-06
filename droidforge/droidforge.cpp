#include "droidforge.h"

DroidForge *the_forge;
DroidFirmware *the_firmware;

DroidForge::DroidForge()
{
    the_forge = this;
    the_firmware = &firmware;
}
