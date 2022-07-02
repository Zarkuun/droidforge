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
    const float faderPositions[4] = { 18.25, 10.35, 14.53, 5.90, };

    float x = 3.50 * (number - 1) + 1.83;
    float y;

    if (type == REGISTER_POT) {
        y = faderPositions[number - 1];
        x -= 0.1;
    }
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
