#ifndef MACMIDIHOST_H
#define MACMIDIHOST_H

#include "patch.h"
#include "tuning.h"

#include <stdint.h>
#include <QString>
#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>


class MacMIDIHost
{
    MIDIPortRef   outputPortRef;
    MIDIClientRef clientRef;
    Byte sysexBuffer[MAX_DROID_INI + 16 + 1000 /* test */];

public:
    MacMIDIHost();
    bool x7Connected() const;
    QString sendPatch(const Patch *patch);
    MIDIEndpointRef findX7() const;

private:
    unsigned prepareSysexMessage(const Patch *patch);
};

#endif // MACMIDIHOST_H
