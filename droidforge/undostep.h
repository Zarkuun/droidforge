#ifndef UNDOSTEP_H
#define UNDOSTEP_H

#include "patch.h"
#include <QString>

class UndoStep
{
    QString name;
    Patch *patch;

private:
    UndoStep(UndoStep &);
    UndoStep(const UndoStep &);

public:
    UndoStep(QString name, const Patch *patch);
    ~UndoStep();
    Patch *getPatch() const { return patch->clone(); };
};

#endif // UNDOSTEP_H
