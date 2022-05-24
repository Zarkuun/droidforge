#include "cursorposition.h"

CursorPosition::CursorPosition()
    : circuitNr(0)
    , row(-2)
    , column(0)
{
}

QDebug &operator<<(QDebug &out, const CursorPosition &pos)
{
    out << QString("%1.%2.%3").arg(pos.circuitNr).arg(pos.row).arg(pos.column);
    return out;
}


bool operator==(const CursorPosition &a, const CursorPosition &b)
{
    return a.circuitNr == b.circuitNr
            && a.row == b.row
            && a.column == b.column;
}
