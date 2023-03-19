#include "moduleg8.h"
#include "tuning.h"

unsigned ModuleG8::numRegisters(register_type_t type) const
{
    if (type == REGISTER_GATE || type == REGISTER_RGB_LED)
        return 8;
    else
        return 0;
}

unsigned ModuleG8::numberOffset(register_type_t type) const
{
    if (type == REGISTER_RGB_LED)
        return 16;
    else
        return 0;
}

QPointF ModuleG8::registerPosition(register_type_t type, unsigned number) const
{
    int column = (number - 1) % 2;
    int row = (number - 1) / 2;

    float x;
    float y;

    if (type == REGISTER_GATE) {
        y = row * 2.55 + 14.47;
        x = column * 2.00 + 0.98;
    }
    else { // REGISTER_RGB_LED
        y = row * 1.775 + 5.88;
        if (row >= 2)
            y += 0.37;
        x = column * 1.775 + 1.15;
    }
    return QPointF(x, y);
}

float ModuleG8::registerSize(register_type_t type, unsigned) const
{
    if (type == REGISTER_RGB_LED)
        return CONTROL_RGBLED_SIZE;
    else
        return CONTROL_JACK_SIZE;
}

float ModuleG8::labelDistance(register_type_t, unsigned) const
{
    return -2.45;
}

float ModuleG8::rectAspect(register_type_t type) const
{
    if (type == REGISTER_RGB_LED)
        return 1.0;
    else
        return 0.0;
}
