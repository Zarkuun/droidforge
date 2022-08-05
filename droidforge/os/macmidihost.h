#ifndef MACMIDIHOST_H
#define MACMIDIHOST_H

#include "midihost.h"
#include <QString>
#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

class MacMIDIHost : public MIDIHost
{
    MIDIPortRef   outputPortRef;
    MIDIClientRef clientRef;

public:
    MacMIDIHost();
    bool x7Connected() const override;
    QString sendPatch(const Patch *patch) override;

protected:
    virtual bool needLostBytesHack() const override { return true; };

private:
    MIDIEndpointRef findX7() const;
};

#endif // MACMIDIHOST_H
