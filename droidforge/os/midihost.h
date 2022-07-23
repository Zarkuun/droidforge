#ifndef MIDIHOST_H
#define MIDIHOST_H

#include <QtGlobal>
#include "os.h"

#ifdef Q_OS_MAC
#include "macmidihost.h"
#define MIDIHost MacMIDIHost
#endif

#endif // MIDIHOST_H
