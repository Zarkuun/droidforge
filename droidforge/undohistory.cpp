#include "undohistory.h"
#include "QtCore/qdebug.h"

UndoHistory::UndoHistory()
{
}

UndoHistory::~UndoHistory()
{
    clear();
}

void UndoHistory::clear()
{

    for (qsizetype i=0; i<steps.size(); i++)
        delete steps[i];
    steps.clear();
}


void UndoHistory::snapshot(QString name, const Patch *patch)
{
    steps.append(new UndoStep(name, patch));
    qDebug() << "Snapshot" << name;
}
