#ifndef INTERACTIVEPATCHOPERATION_H
#define INTERACTIVEPATCHOPERATION_H

#include "patch.h"

class InteractivePatchOperation
{
public:
    static bool interactivelyRemapRegisters(Patch *patch, Patch *otherpatch);
};

#endif // INTERACTIVEPATCHOPERATION_H
