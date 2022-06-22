#ifndef CURSORPOSITION_H
#define CURSORPOSITION_H

#include <QDebug>

#define ROW_CIRCUIT -2
#define ROW_COMMENT -1

struct CursorPosition
{
    int circuitNr;
    int row;
    int column;

    CursorPosition();
    CursorPosition(int c, int r, int co) : circuitNr(c), row(r), column(co) {};
    bool isAtAtom() const;
};

QDebug &operator<<(QDebug &out, const CursorPosition &pos);
bool operator==(const CursorPosition &a, const CursorPosition &b);
bool operator!=(const CursorPosition &a, const CursorPosition &b);

#endif // CURSORPOSITION_H
