#include "cursorposition.h"

CursorPosition::CursorPosition()
    : circuitNr(0)
    , row(ROW_CIRCUIT)
    , column(0)
{
}

bool CursorPosition::isAtAtom() const
{
    return row >= 0 && column >= 1;
}

QString CursorPosition::toString() const
{
    return QString("%1.%2.%3").arg(circuitNr).arg(row).arg(column);
}

QDebug &operator<<(QDebug &out, const CursorPosition &pos)
{
    out << pos.toString();
    return out;
}


bool operator==(const CursorPosition &a, const CursorPosition &b)
{
    return a.circuitNr == b.circuitNr
            && a.row == b.row
            && a.column == b.column;
}


bool operator!=(const CursorPosition &a, const CursorPosition &b)
{
    return !(a == b);
}
