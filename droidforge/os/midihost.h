#ifndef MIDIHOST_H
#define MIDIHOST_H

#include <QtGlobal>
#include "os.h"
#include "patch.h"
#include "tuning.h"

class MIDIHost
{
    uint8_t sysexBuffer[MAX_DROID_INI + 16 + 1000 /* test */]; // TODO:Korrekte Groesse

public:
    MIDIHost();
    virtual bool x7Connected() const = 0;
    virtual QString sendPatch(const Patch *patch) = 0;

protected:
    uint8_t *sysexData() { return &sysexBuffer[0]; };
    unsigned prepareSysexMessage(const Patch *patch);
};


#endif // MIDIHOST_H
