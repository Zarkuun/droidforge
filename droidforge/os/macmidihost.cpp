#include <QtGlobal>
#ifdef Q_OS_MAC
#include "macmidihost.h"
#include "globals.h"

#include <QDateTime>
#include <QThread>


bool sysexOngoing = false;

void sysex_complete(MIDISysexSendRequest *)
{
    sysexOngoing = false;
}

MacMIDIHost::MacMIDIHost()
    : outputPortRef(0)
    , clientRef(0)
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
    // Wait until a previous Sysex has finished. Otherwise
    // the data structures get wasted and we crash. Yes, this
    // can happen, if the user falls asleep while holding the F9
    // key ;-)
    while (sysexOngoing) {
        QThread::msleep(10);
    }

    unsigned sysexLength = prepareSysexMessage(patch);
    if (sysexLength == 0)
        return TR("You have exceeded the maximum allowed patch size.");

    MIDIEndpointRef endpointRef = findX7();
    if (!endpointRef)
        return TR("Cannot find DROID X7");

    bzero(&req, sizeof(req));
    req.destination = endpointRef;
    req.data = sysexData();
    req.bytesToSend = sysexLength;
    req.complete = false; // Wird von außen gesetzt?
    req.completionProc = sysex_complete; // Callback
    req.completionRefCon = (void *)this; // Ein pointer für den Callbar
    MIDISendSysex(&req);

    sysexOngoing = true;
    CFRunLoopRun();
    return "";
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
