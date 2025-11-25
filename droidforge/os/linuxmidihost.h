#ifndef LINUXMIDIHOST_H
#define LINUXMIDIHOST_H

#include "midihost.h"

#include <stdint.h>
#include <QString>

class LinuxMIDIHost : public MIDIHost
{
public:
    LinuxMIDIHost() {};
    bool x7Connected() const override;
    QString sendPatch(const Patch *patch) override;

private:
    int findX7Device() const;
};

#endif // LINUXMIDIHOST_H