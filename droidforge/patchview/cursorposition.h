#ifndef CURSORPOSITION_H
#define CURSORPOSITION_H

#include <QDebug>

struct CursorPosition
{
    int circuitNr;
    int row;
    int column;

    CursorPosition();
};

QDebug &operator<<(QDebug &out, const CursorPosition &pos);

#endif // CURSORPOSITION_H
