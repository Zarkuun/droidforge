#include "moduleg8.h"
#include "tuning.h"

unsigned ModuleG8::numRegisters(QChar type) const
{
    if (type == REGISTER_GATE || type == REGISTER_RGB_LED)
        return 8;
    else
        return 0;
}

unsigned ModuleG8::numberOffset(QChar type) const
{
    if (type == REGISTER_RGB_LED)
        return 16;
    else
        return 0;
}

QPointF ModuleG8::registerPosition(QChar type, unsigned number) const
{
    int column = (number - 1) % 2;
    int row = (number - 1) / 2;

    float x;
    float y;

    if (type == REGISTER_GATE) {
        y = row * 2.55 + 14.5;
        x = column * 2.05 + 0.9;
    }
    else { // REGISTER_RGB_LED
        y = row * 1.8 + 5.8;
        if (row >= 2)
            y += 0.3;
        x = column * 1.85 + 1.2;
    }
    return QPointF(x, y);
}

float ModuleG8::registerSize(QChar type, unsigned) const
{
    if (type == REGISTER_RGB_LED)
        return CONTROL_RGBLED_SIZE;
    else
        return CONTROL_JACK_SIZE;
}
