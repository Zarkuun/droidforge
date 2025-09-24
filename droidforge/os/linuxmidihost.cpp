#include "os.h"
#ifdef Q_OS_LINUX
#include "linuxmidihost.h"
#include "globals.h"

#include <QThread>

bool LinuxMIDIHost::x7Connected() const
{
    return findX7Device() >= 0;
}

int LinuxMIDIHost::findX7Device() const
{
    return -1;
}

QString LinuxMIDIHost::sendPatch(const Patch *patch)
{
    (void) patch;
    return TR("Could not find DROID X7 MIDI device");
}
#endif
