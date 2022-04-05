#include "moduleb32.h"

ModuleB32::ModuleB32()
{

}

QString ModuleB32::name() { return "b32"; }

QString ModuleB32::faceplate() { return "faceplate-b32-off"; }

QString ModuleB32::title() { return "B32 Controller"; }

unsigned ModuleB32::hp() { return 10; }
