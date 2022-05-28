#ifndef CURSORPOSITION_H
#define CURSORPOSITION_H

#include <QDebug>

struct CursorPosition
{
    int circuitNr;
    int row;
    int column;

    CursorPosition();
    CursorPosition(int c, int r, int co) : circuitNr(c), row(r), column(co) {};
};

QDebug &operator<<(QDebug &out, const CursorPosition &pos);
bool operator==(const CursorPosition &a, const CursorPosition &b);

#endif // CURSORPOSITION_H
