#include "modulem4.h"
#include "tuning.h"

unsigned ModuleM4::numControls(QChar type) const
{
    if (type == 'P' || type == 'B' || type == 'L')
        return 4;
    else
        return 0;
}

QPointF ModuleM4::controlPosition(QChar type, unsigned number)
{
    const float faderPositions[4] = {
        17.5,
        9.5,
        14.6,
        6.1,
    };

    float x = 3.56 * (number - 1) + 1.65;
    float y;

    if (type == 'P')
        y = faderPositions[number - 1];
    else
        y = 23.0;
    return QPointF(x, y);
}


float ModuleM4::controlSize(QChar type, unsigned)
{
    if (type == 'P')
        return CONTROL_M4_FADER_SIZE;
    else
        return CONTROL_M4_TOUCH_SIZE;
}
