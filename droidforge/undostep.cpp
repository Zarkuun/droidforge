#include "undostep.h"


UndoStep::UndoStep(QString name, const Patch *patch)
    : name(name)
    , patch(patch->clone())
{
}


UndoStep::~UndoStep()
{
    delete patch;
}
