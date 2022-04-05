#include "moduleg8.h"

ModuleG8::ModuleG8()
{

}

QString ModuleG8::name()  { return "g8"; }

QString ModuleG8::faceplate() {return "faceplate-g8-off"; }

QString ModuleG8::title() { return "G8 Gates Expander"; }

unsigned ModuleG8::hp() { return 4; }
