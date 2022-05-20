#include "modulem4.h"
#include "tuning.h"

unsigned ModuleM4::numRegisters(QChar type) const
{
    if (type == REGISTER_POT || type == REGISTER_BUTTON || type == REGISTER_LED)
        return 4;
    else
        return 0;
}

QPointF ModuleM4::registerPosition(QChar type, unsigned number) const
{
    const float faderPositions[4] = {
        17.5,
        9.5,
        14.6,
        6.1,
    };

    float x = 3.56 * (number - 1) + 1.65;
    float y;

    if (type == REGISTER_POT)
        y = faderPositions[number - 1];
    else
        y = 23.0;
    return QPointF(x, y);
}

float ModuleM4::registerSize(QChar type, unsigned) const
{
    if (type == REGISTER_POT)
        return CONTROL_M4_FADER_SIZE;
    else
        return CONTROL_M4_TOUCH_SIZE;
}
