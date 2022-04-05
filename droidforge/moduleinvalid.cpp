#include "moduleinvalid.h"

ModuleInvalid::ModuleInvalid()
{

}

QString ModuleInvalid::name() { return "invalid"; }

QString ModuleInvalid::faceplate() { return "faceplate-blind-front.jpg"; }

QString ModuleInvalid::title() { return "Invalid"; }

unsigned ModuleInvalid::hp() { return 1; }
