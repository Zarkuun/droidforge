#include "modulem4.h"
#include "tuning.h"

unsigned ModuleM4::numRegisters(register_type_t type) const
{
    if (type == REGISTER_POT || type == REGISTER_BUTTON || type == REGISTER_LED)
        return 4;
    else
        return 0;
}

QPointF ModuleM4::registerPosition(register_type_t type, unsigned number) const
{
    // const float faderPositions[4] = { 18.25, 10.35, 14.53, 5.90, };

    float x = 3.50 * (number - 1) + 1.83;
    float y;

    if (type == REGISTER_POT) {
        y = 18.27;
        x -= 0.1;
    }
    else
        y = 23.0;
    return QPointF(x, y);
}

float ModuleM4::registerSize(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return CONTROL_M4_FADER_SIZE;
    else
        return CONTROL_M4_TOUCH_SIZE;
}

float ModuleM4::labelDistance(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return -15.34;
    else
        return -2.35;
}

float ModuleM4::labelWidth(register_type_t, unsigned) const
{
    return 3.4;
}
