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

QDebug &operator<<(QDebug &out, const PatchProblem &pb)
{
    return out << pb.toString();
}
