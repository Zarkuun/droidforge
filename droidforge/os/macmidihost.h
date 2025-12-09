#ifndef MACMIDIHOST_H
#define MACMIDIHOST_H

#include "midihost.h"

#include <QString>
#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>
#include <QMutex>
#include <QWaitCondition>

class MacMIDIHost : public MIDIHost
{
    MIDIPortRef   outputPortRef;
    MIDIClientRef clientRef;
    bool sysexOngoing;
    QMutex sysexMutex;
    QWaitCondition sysexWait;

public:
    MacMIDIHost();
    bool x7Connected() const override;
    QString sendPatch(const Patch *patch) override;
    void sysexComplete();

protected:
    virtual bool needLostBytesHack() const override { return true; };

private:
    MIDIEndpointRef findX7() const;
};

#endif // MACMIDIHOST_H
