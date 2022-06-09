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
    req.data = sysexBuffer;
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
unsigned MacMIDIHost::prepareSysexMessage(const Patch *patch)
{
    sysexBuffer[0] = 0xf0;
    sysexBuffer[1] = 0x00;
    sysexBuffer[2] = 0x66;
    sysexBuffer[3] = 0x66;
    sysexBuffer[4] = 'P';
    QString droidini = patch->toBare();
    const char *iniAsCstring = droidini.toUtf8();
    unsigned patchLen = strlen(iniAsCstring);
    if (patchLen > MAX_DROID_INI)
        return 0;

    // The MIDI Driver of MAC seems to have hickups if the length
    // of a sysex message is > 255 bytes. It drops each 256th byte.
    // Strange. I dont' know wether this is a bug or part of the specs
    // somewhere. So we simply insert one space byte every 256th byte
    // (which will then happily get lost on the way)
    #define SYSEX_MAX_CHUNK 255

    const char *read = iniAsCstring;
    Byte *write = &sysexBuffer[5];
    int remaining = patchLen;
    int maxChunkLen = SYSEX_MAX_CHUNK - 5;
    while (remaining) {
        int chunk = qMin(remaining, maxChunkLen);
        remaining -= chunk;
        memcpy(write, read, chunk);
        write += chunk;
        read += chunk;
        *write++ = ' ';
        maxChunkLen = SYSEX_MAX_CHUNK;
    }
    *write++ = 0xf7;

    int totalLen = write - &sysexBuffer[0];
    return totalLen;
}
