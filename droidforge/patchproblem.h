#ifndef PATCHPROBLEM_H
#define PATCHPROBLEM_H

#include "atomregister.h"
#include "cursorposition.h"

#include <QDebug>

class PatchProblem
{
    int section;
    CursorPosition curPos;
    QString reason;

public:
    PatchProblem(int row, int column, const QString &reason);
    QString toString() const;

    int getSection() const { return section; };
    void setSection(int newSection) { section = newSection; };
    void setCircuit(int circuitNr) { curPos.circuitNr = circuitNr; };
    void setRow(int row) { curPos.row = row; };
    const CursorPosition &getCursorPosition() const { return curPos; };
    const QString &getReason() const { return reason; };
};

QDebug &operator<<(QDebug &out, const PatchProblem &pb);

#endif // PATCHPROBLEM_H
