#include "modulex7.h"
#include "tuning.h"

unsigned ModuleX7::numRegisters(QChar type) const
{
    if (type == REGISTER_GATE)
        return 4;
    else if (type == REGISTER_RGB_LED)
        return 8;
    else
        return 0;
}

QPointF ModuleX7::registerPosition(QChar type, unsigned number) const
{
    int column = (number - 1) % 2;
    int row = (number - 1) / 2;

    float x;
    float y;

    if (type == REGISTER_GATE) {
        x = column * 2.05 + 0.98;
        y = row * 2.55 + 19.6;
    }
    else { // REGISTER_RGB
        y = row * 1.8 + 5.8;
        if (row >= 2)
            y += 0.3;
        x = column * 1.85 + 1.1;
    }
    return QPointF(x, y);

}

float ModuleX7::registerSize(QChar type, unsigned) const
{
    if (type == REGISTER_RGB_LED)
        return CONTROL_RGBLED_SIZE;
    else
        return CONTROL_JACK_SIZE;

}

unsigned ModuleX7::numberOffset(QChar type) const
{
    if (type == REGISTER_GATE)
        return 8;
    else if (type == REGISTER_RGB_LED)
        return 24;
    else
        return 0;
}
