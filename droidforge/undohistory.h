#ifndef UNDOHISTORY_H
#define UNDOHISTORY_H

#include "patch.h"
#include "undostep.h"

#include <QList>

class UndoHistory
{
    QList<UndoStep *> steps;

public:
    UndoHistory();
    ~UndoHistory();
    void clear();
    void snapshot(QString name, const Patch *patch);
    qsizetype size() const { return steps.size(); };
    Patch *undo();
};

#endif // UNDOHISTORY_H
