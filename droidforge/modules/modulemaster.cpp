#include "modulemaster.h"
#include "tuning.h"


unsigned ModuleMaster::numRegisters(QChar type) const
{
    if (type == REGISTER_INPUT || type == REGISTER_OUTPUT || type == REGISTER_NORMALIZE)
        return 8;
    else if (type == REGISTER_RGB_LED)
        return 16;
    else if (type == REGISTER_EXTRA)
        return 1;
    else
        return 0;
}

QPointF ModuleMaster::registerPosition(QChar type, unsigned number) const
{
    int column = (number - 1) % 4;
    int row = (number - 1) / 4;

    float y;
    float x = column * 2.00 + 0.98;

    if (type == REGISTER_INPUT || type == REGISTER_NORMALIZE)
        y = row * 2.55 + 14.47;
    else if (type == REGISTER_OUTPUT)
        y = row * 2.55 + 19.57;
    else if (type == REGISTER_RGB_LED) {
        y = row * 1.775 + 5.88;
        if (row >= 2)
            y += 0.37;
        x = column * 1.775 + 1.35;
    }
    else {
        return QPointF(4.0, 8.55);
    }
    return QPointF(x, y);
}

float ModuleMaster::registerSize(QChar type, unsigned) const
{
    if (type == REGISTER_RGB_LED)
        return CONTROL_RGBLED_SIZE;
    else if (type == REGISTER_EXTRA)
        return CONTROL_EXTRA_SIZE;
    else
        return CONTROL_JACK_SIZE;
}
