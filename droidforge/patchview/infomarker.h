#ifndef INFOMARKER_H
#define INFOMARKER_H

#include "cursorposition.h"
#include "iconmarker.h"

class InfoMarker : public IconMarker
{
    CursorPosition curPos;

public:
    InfoMarker(const CursorPosition &curPos, unsigned size, QString toolTip);
    const CursorPosition &cursorPosition() const { return curPos; };
};

#endif // INFOMARKER_H
