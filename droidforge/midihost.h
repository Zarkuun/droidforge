#ifndef MIDIHOST_H
#define MIDIHOST_H

#include "patch.h"
#include "tuning.h"

#include <stdint.h>
#include <QString>
#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>


class MIDIHost
{
    MIDIPortRef   outputPortRef;
    MIDIClientRef clientRef;
    Byte sysexBuffer[MAX_DROID_INI + 16];
    uint64_t lastTime;

public:
    MIDIHost();
    // bool x7Connected();
    QString sendPatch(const Patch *patch);
    MIDIEndpointRef findX7() const;

private:
    unsigned prepareSysexMessage(const Patch *patch);
};

#endif // MIDIHOST_H
