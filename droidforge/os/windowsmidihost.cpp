#include <windows.h>
#include "windowsmidihost.h"
#include "globals.h"

bool WindowsMIDIHost::x7Connected() const
{
    return false;
    /*
    UINT nMidiDeviceNum;
    MIDIINCAPS caps;

    nMidiDeviceNum = midiInGetNumDevs();
    if (nMidiDeviceNum == 0) {
        fprintf(stderr, "midiInGetNumDevs() return 0...");
        return false;
    }

    shout << "== PrintMidiDevices() == \n";
    for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
        midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
        shout << "\t%d : name = %s\n" << i << caps.szPname;
    }
    shout << "=====";
    return true; */
}

QString MIDIHost::sendPatch(const Patch *patch)
{
    return QString("das hat nicht geklappt");
}
