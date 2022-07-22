#ifndef MIDIHOST_H
#define MIDIHOST_H

#include <QtGlobal>

#ifdef Q_MAC_OS
#include "macmidihost.h"
#define MIDIHost MacMIDIHost
#endif

#endif // MIDIHOST_H
