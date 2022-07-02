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
        y = (row + 2) * 2.55 + 14.47;
        x = column * 2.00 + 0.98;
    }
    else { // REGISTER_RGB
        y = row * 1.775 + 5.88;
        if (row >= 2)
            y += 0.37;
        x = column * 1.775 + 1.15;
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
