#ifndef WINDOWSMIDIHOST_H
#define WINDOWSMIDIHOST_H

#include "midihost.h"

#include <stdint.h>
#include <QString>

class WindowsMIDIHost : public MIDIHost
{
public:
    WindowsMIDIHost() {};
    bool x7Connected() const override;
    QString sendPatch(const Patch *patch) override;

private:
    int findX7Device() const;
};

#endif // WINDOWSMIDIHOST_H
