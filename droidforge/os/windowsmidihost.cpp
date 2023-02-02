#include "os.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include "windowsmidihost.h"
#include "globals.h"

#include <QThread>

// https://docs.microsoft.com/en-us/windows/win32/api/_multimedia/

// Needs Winmm.lib / Winmm.dll

bool WindowsMIDIHost::x7Connected() const
{
    return findX7Device() >= 0;
}

int WindowsMIDIHost::findX7Device() const
{
    MIDIOUTCAPS caps;
    UINT nMidiDeviceNum = midiOutGetNumDevs();
    for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
        midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));
        QString name = QString::fromWCharArray(caps.szPname);
        if (name.contains("X7"))
            return i;
    }
    return -1;
}

QString WindowsMIDIHost::sendPatch(const Patch *patch)
{
    int deviceId = findX7Device();
    if (deviceId < 0)
        return TR("Could not find DROID X7 MIDI device");

    HMIDIOUT handle;
    int result = midiOutOpen(&handle, (UINT)deviceId, NULL, 0, CALLBACK_NULL);
    if (result != MMSYSERR_NOERROR)
        return TR("Could not open X7 MIDI device");

    unsigned length = prepareSysexMessage(patch);
    MIDIHDR midiHdr;

    midiHdr.lpData = (char *)sysexData();
    midiHdr.dwBufferLength = length;
    midiHdr.dwFlags = 0;

    /* Prepare the buffer and MIDIHDR */
    UINT err = midiOutPrepareHeader(handle,  &midiHdr, sizeof(MIDIHDR));
    if (!err)
    {
        /* Output the SysEx message */
        err = midiOutLongMsg(handle, &midiHdr, sizeof(MIDIHDR));
        if (err)
        {
            wchar_t errMsg[120];
            midiOutGetErrorText(err, &errMsg[0], 120);
            QString err = QString::fromWCharArray(errMsg);
            midiOutClose(handle);
            return err;
        }

        /* Unprepare the buffer and MIDIHDR */
        while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR)))
            QThread::msleep(100);
    }

    midiOutClose(handle);
    return "";
}
#endif
