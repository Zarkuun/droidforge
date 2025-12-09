#include <QtGlobal>
#ifdef Q_OS_MAC
#include "macmidihost.h"
#include "globals.h"

#include <QDateTime>
#include <QThread>

void sysex_complete(MIDISysexSendRequest *request);

MacMIDIHost::MacMIDIHost()
    : outputPortRef(0)
    , clientRef(0)
    , sysexOngoing(false)
{
    MIDIClientCreate(CFSTR("DROID Forge"), NULL, NULL, &clientRef);
    MIDIOutputPortCreate(clientRef, CFSTR("Output port"), &outputPortRef);
}

bool MacMIDIHost::x7Connected() const
{
    return findX7() != 0;
}

// Strange. But this struct seems to need to be global. Otherwise
// we got crashed in the CoreMIDI Foundation from time to time.
struct MIDISysexSendRequest req;


QString MacMIDIHost::sendPatch(const Patch *patch)
{
    QMutexLocker locker(&sysexMutex);

    while (sysexOngoing)
        sysexWait.wait(&sysexMutex);

    unsigned sysexLength = prepareSysexMessage(patch);
    if (sysexLength == 0)
        return TR("You have exceeded the maximum allowed patch size.");

    MIDIEndpointRef endpointRef = findX7();
    if (!endpointRef)
        return TR("Cannot find DROID X7");

    // SysEx Request füllen
    memset(&req, 0, sizeof(req));
    req.destination = endpointRef;
    req.data = sysexData();
    req.bytesToSend = sysexLength;
    req.complete = false;
    req.completionProc = sysex_complete;
    req.completionRefCon = this;

    sysexOngoing = true;
    MIDISendSysex(&req);
    sysexWait.wait(&sysexMutex);

    return "";
}

void sysex_complete(MIDISysexSendRequest *request)
{
    MacMIDIHost* host = static_cast<MacMIDIHost*>(request->completionRefCon);
    host->sysexComplete();
}

void MacMIDIHost::sysexComplete()
{
    sysexMutex.lock();
    sysexOngoing = false;
    sysexWait.wakeAll();
    sysexMutex.unlock();
}


MIDIEndpointRef MacMIDIHost::findX7() const
{
    int numDestinations = MIDIGetNumberOfDestinations();

    CFStringRef pname;
    char name[64];
    MIDIEndpointRef destinationRef = 0;

    for (int i=0; i<numDestinations; i++) {
        destinationRef = MIDIGetDestination(i); // Das ist eine Endpointref!!!
        if (destinationRef != 0) {
            MIDIObjectGetStringProperty(destinationRef, kMIDIPropertyName, &pname);
            CFStringGetCString(pname, name, sizeof(name), 0);
            CFRelease(pname);

            QString destinationName(name);
            if (destinationName.contains("DROID X7 MIDI")) {
                return destinationRef;
            }
        }
    }
    return 0;
}
#endif
