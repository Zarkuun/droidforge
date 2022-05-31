#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "patch.h"
#include <QString>

class PatchVersion
{
    QString name;
    Patch *patch; // lives here

private:
    PatchVersion(PatchVersion &);
    PatchVersion(const PatchVersion &);

public:
    PatchVersion(QString name, const Patch *patch); // patch is cloned
    ~PatchVersion();
    QString getName() const { return name; };
    const Patch *getPatch() const { return patch; };
};

#endif // EDITORSTATE_H
