#include "selection.h"

Selection::Selection(const CursorPosition &fromPos, const CursorPosition &toPos)
{
    // normalize
    bool swap;
    if (fromPos.circuitNr > toPos.circuitNr)
        swap = true;
    else if (fromPos.circuitNr == toPos.circuitNr) {
        if (fromPos.row > toPos.row)
            swap = true;
        else if (fromPos.row == toPos.row)
            swap = fromPos.column > toPos.column;
        else
            swap = false;
    }
    else
        swap = false;


    if (swap) {
        from = toPos;
        to = fromPos;
    }
    else {
        from = fromPos;
        to = toPos;
    }
}

Selection::Selection(const CursorPosition &fromAndto)
    : Selection(fromAndto, fromAndto)
{
}

bool Selection::circuitSelected(int circuitNr) const
{
    return isCircuitSelection()
            &&  circuitNr >= from.circuitNr
            &&  circuitNr <= to.circuitNr;
}

bool Selection::commentSelected(int circuitNr) const
{
    return
            from.circuitNr == circuitNr &&
            to.circuitNr == circuitNr &&
            from.row == -1
            && to.row == -1;
}

bool Selection::jackSelected(int circuitNr, int row) const
{
    return from.circuitNr == circuitNr &&
            to.circuitNr == circuitNr &&
            from.row >= 0 &&
            row >= from.row &&
            row <= to.row &&
            !isAtomSelection();
}

bool Selection::atomSelected(int circuitNr, int row, int atom) const
{
    return isAtomSelection() &&
            from.circuitNr == circuitNr &&
            from.row == row &&
            atom >= from.column &&
            atom <= to.column;
}

bool Selection::isAtomSelection() const
{
    return from.circuitNr == to.circuitNr &&
            from.row >= 0 &&
            from.row == to.row &&
            from.column > 0;
}

bool Selection::isSingleAtomSelection() const
{
    return isAtomSelection() &&
            from.column == to.column;
}

bool Selection::isJackSelection() const
{
    return from.circuitNr == to.circuitNr &&
            from.row >= 0 &&
            (from.column == 0 ||
             from.row != to.row);
}

bool Selection::isSingleJackSelection() const
{
    return isJackSelection() &&
            from.row == to.row;
}

bool Selection::isCircuitSelection() const
{
    // Different circuits -> always select complete circuit
    if (from.circuitNr != to.circuitNr)
        return true;

    // Circuit header is part of selection
    if (from.row == ROW_CURSOR)
        return true;

    // Comment and non-comment are part of selection
    if (from.row == -1 && to.row != -1)
        return true;

    return false;
}

bool Selection::isSingleCircuitSelection() const
{
    return isCircuitSelection() && from.circuitNr == to.circuitNr;
}

bool Selection::isCommentSelection() const
{
    return from.circuitNr == to.circuitNr &&
            from.row == -1 && to.row == -1;
}
