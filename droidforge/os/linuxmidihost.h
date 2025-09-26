#ifndef LINUXMIDIHOST_H
#define LINUXMIDIHOST_H

#include "midihost.h"

#include <stdint.h>
#include <QString>

struct alsaSeq;

class LinuxMIDIHost : public MIDIHost
{
public:
    LinuxMIDIHost() {};
    bool x7Connected() const override;
    QString sendPatch(const Patch *patch) override;

private:
    int findX7(int *) const;
    bool connect(int, int);
    void disconnect(int, int);
    void send(uint8_t*, unsigned);

    alsaSeq *alsa;
};

#endif // LINUXMIDIHOST_H
