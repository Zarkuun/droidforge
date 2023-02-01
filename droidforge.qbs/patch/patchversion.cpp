#include "patchversion.h"

PatchVersion::PatchVersion(QString name, const Patch *patch)
    : name(name)
    , patch(patch->clone())
{
}

PatchVersion::~PatchVersion()
{
    delete patch;
}
