#include "midihost.h"

MIDIHost::MIDIHost()
{

}

unsigned MIDIHost::prepareSysexMessage(const Patch *patch)
{
    sysexBuffer[0] = 0xf0;
    sysexBuffer[1] = 0x00;
    sysexBuffer[2] = 0x66;
    sysexBuffer[3] = 0x66;
    sysexBuffer[4] = 'P';
    QString droidini = patch->toBare();

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
        // Hack for MacOS that is missing one byte every 255 bytes
        if (offset == SYSEX_MAX_CHUNK) {
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
