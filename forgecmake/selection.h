#ifndef SELECTION_H
#define SELECTION_H

#include "cursorposition.h"

class Selection
{
    CursorPosition from, to;

public:
    Selection(const CursorPosition &from, const CursorPosition &to);
    Selection(const CursorPosition &fromAndto);
    bool circuitSelected(int n) const;
    bool commentSelected(int n) const;
    bool jackSelected(int n, int r) const;
    bool atomSelected(int n, int r, int a) const;
    const CursorPosition &fromPos() const { return from; };
    const CursorPosition &toPos() const { return to; };
    bool isAtomSelection() const;
    bool isSingleAtomSelection() const;
    bool isJackSelection() const;
    bool isSingleJackSelection() const;
    bool isCircuitSelection() const;
    bool isSingleCircuitSelection() const;
    bool isCommentSelection() const;

private:
};

#endif // SELECTION_H
