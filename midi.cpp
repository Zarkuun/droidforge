/*  Copyright © 2007 Apple Inc. All Rights Reserved.  */

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>
#include <stdio.h>

// ___________________________________________________________________________________________
// test program to echo MIDI In to Out
// ___________________________________________________________________________________________

MIDIPortRef     gOutPort = 0;
MIDIEndpointRef gDest = 0;


void sysex_complete(MIDISysexSendRequest *req)
{
    printf("Request %p fertig!\n", req);
    exit(0);
}

int main(int argc, char *argv[])
{
    printf("MAIN");

    // create client and ports
    MIDIClientRef client = 0;
    MIDIClientCreate(CFSTR("DROID Forge"), NULL, NULL, &client);

    printf("eins");

    MIDIPortRef inPort = 0;
    MIDIOutputPortCreate(client, CFSTR("Output port"), &gOutPort);

    printf("zwei");

    // enumerate devices (not really related to purpose of the echo program
    // but shows how to get information about devices)
    int i, n;
    CFStringRef pname, pmanuf, pmodel;
    char name[64], manuf[64], model[64];

    printf("drei");
    n = MIDIGetNumberOfDevices();

    for (i = 0; i < n; ++i) {
        printf("vier: %d", i);
        MIDIDeviceRef dev = MIDIGetDevice(i);

        MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &pname);
        MIDIObjectGetStringProperty(dev, kMIDIPropertyManufacturer, &pmanuf);
        MIDIObjectGetStringProperty(dev, kMIDIPropertyModel, &pmodel);

        CFStringGetCString(pname, name, sizeof(name), 0);
        CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
        CFStringGetCString(pmodel, model, sizeof(model), 0);
        CFRelease(pname);
        CFRelease(pmanuf);
        CFRelease(pmodel);

        printf("name=%s, manuf=%s, model=%s\n", name, manuf, model);
    }

    // open connections from all sources
    // n = MIDIGetNumberOfSources();
    // printf("%d sources\n", n);
    // for (i = 0; i < n; ++i) {
    //     MIDIEndpointRef src = MIDIGetSource(i);
    //     MIDIPortConnectSource(inPort, src, NULL);
    // }

    int ext = MIDIGetNumberOfExternalDevices();
    printf("External devices: %d\n", ext); // ist komischerweise 0

    // find the first destination
    n = MIDIGetNumberOfDestinations();
    for (int i=0; i<n; i++) {
        gDest = MIDIGetDestination(i); // Das ist eine Endpointref!!!

        if (gDest != 0) {
            MIDIObjectGetStringProperty(gDest, kMIDIPropertyName, &pname);
            CFStringGetCString(pname, name, sizeof(name), 0);
            CFRelease(pname);

            // func MIDISendSysex(_ request: UnsafeMutablePointer<MIDISysexSendRequest>) -> OSStatus
            const char *patch = "[lfo]\nsquare = O1\nhz = 10\n";
            Byte data[100];
            data[0] = 0xf0;
            data[1] = 0x00;
            data[2] = 0x66;
            data[3] = 0x66;
            data[4] = 'P';
            memcpy(&data[5], patch, strlen(patch));
            data[strlen(patch) + 5] = 0xf7;
            int total_length = strlen(patch) + 6;

            struct MIDISysexSendRequest req;
            req.destination = gDest;
            req.data = data;
            req.bytesToSend = total_length;
            req.complete = false; // Wird von außen gesetzt?
            req.completionProc = sysex_complete; // Callback
            req.completionRefCon = (void *)"HALLO"; // Ein pointer für den Callbar

            printf("Ich sende nach %s\n", name);
            MIDISendSysex(&req);
        } else {
            printf("MIDI destination %d is dead\n", i);
        }
    }

    CFRunLoopRun();
    // run until aborted with control-C

    return 0;
}
