#ifndef WINDOWSMIDIHOST_H
#define WINDOWSMIDIHOST_H

#include "patch.h"
#include "tuning.h"

#include <stdint.h>
#include <QString>

class WindowsMIDIHost
{
    uint8_t sysexBuffer[MAX_DROID_INI + 16 + 1000 /* test */];

public:
    WindowsMIDIHost() {};
    bool x7Connected() const { return false; }
    QString sendPatch(const Patch *patch) { return ""; };
};

#endif // WINDOWSMIDIHOST_H
