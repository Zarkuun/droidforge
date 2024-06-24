#include "midihost.h"

MIDIHost::MIDIHost()
{
}
unsigned MIDIHost::prepareSysexMessage(const Patch *patch)
{
    QString droidini = patch->toDeployString();
    if (droidini.length() > MAX_DROID_INI)
        return 0;

    sysexBuffer[0] = 0xf0;
    sysexBuffer[1] = 0x00;
    sysexBuffer[2] = 0x66;
    sysexBuffer[3] = 0x66;
    sysexBuffer[4] = 'P';

    // The MIDI Driver of MAC seems to have hickups if the length
    // of a sysex message is > 255 bytes. It drops each 256th byte.
    // Strange. I dont' know wether this is a bug or part of the specs
    // somewhere. So we simply insert one space byte every 256th byte
    // (which will then happily get lost on the way)
    #define SYSEX_MAX_CHUNK 255

    unsigned i=0;
    unsigned offset = 5;
    uint8_t *write = &sysexBuffer[5];
    while (i < droidini.size()) {
        // Hack for MacOS that is missing one byte every 255 bytes.
        // Meanwhile I have found out that Mac is indeed *sending* these
        // bytes, albeit not in the normal SYSEX-packets but in special
        // packet of the type "single byte" (nibble 0xf). Maybe the reason
        // is that it wants to pad everything to 256 bytes and the sysex
        // packets contain 3 bytes each. So after sending 255 bytes one byte
        // is left and is send "out of band" as a single byte.
        // Neither Droid master nor X7 handle these "single bytes", sorry. So
        // we insert one bogus space at exactly the positions where this single
        // byte is packed. If the spaces get lost, it doesn't matter. If it
        // is sent (Windows), if will be stripped by the Droid anyway.
        if (needLostBytesHack() && offset == SYSEX_MAX_CHUNK) {
            *write++ = ' ';
            offset = 0;
        }
        *write++ = droidini[i++].toLatin1();
        offset++;
    }
    *write++ = 0xf7;

    int totalLen = write - &sysexBuffer[0];
    return totalLen;
}
