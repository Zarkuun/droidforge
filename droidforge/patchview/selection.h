#ifndef SELECTION_H
#define SELECTION_H

#include "cursorposition.h"

class Selection
{
    CursorPosition from, to;

public:
    Selection(const CursorPosition &from, const CursorPosition &to);
    bool circuitSelected(int n) const;
    bool commentSelected(int n) const;
    bool jackSelected(int n, int r) const;
    bool atomSelected(int n, int r, int a) const;

private:
    bool isAtomSelection() const;
    bool isCircuitSelection() const;
};

#endif // SELECTION_H
