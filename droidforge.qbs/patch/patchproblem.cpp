#include "patchproblem.h"

PatchProblem::PatchProblem(int row, int column, const QString &reason)
    : section(-1)
    , curPos{-1, row, column}
    , reason(reason)
{
}

QString PatchProblem::toString() const
{
    QString text = QString("Section %1 Cursor: %2.%3.%4: %5")
            .arg(section).arg(curPos.circuitNr).arg(curPos.row).arg(curPos.column).arg(reason);
    return text;
}

bool PatchProblem::isAt(int sec, const CursorPosition &pos) const
{
    return section == sec && curPos == pos;
}

QDebug &operator<<(QDebug &out, const PatchProblem &pb)
{
    return out << pb.toString();
}
