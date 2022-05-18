#include "modulemaster.h"
#include "tuning.h"


unsigned ModuleMaster::numRegisters(QChar type) const
{
    if (type == REGISTER_INPUT || type == REGISTER_OUTPUT || type == REGISTER_NORMALIZE)
        return 8;
    else if (type == REGISTER_RGB)
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
    float x = column * 2.05 + 0.9;

    if (type == REGISTER_INPUT || type == REGISTER_NORMALIZE)
        y = row * 2.55 + 14.5;
    else if (type == REGISTER_OUTPUT)
        y = row * 2.55 + 19.6;
    else if (type == REGISTER_RGB) {
        y = row * 1.8 + 5.8;
        if (row >= 2)
            y += 0.3;
        x = column * 1.85 + 1.3;
    }
    else {
        return QPointF(4.0, 8.55);
    }
    return QPointF(x, y);
}

float ModuleMaster::registerSize(QChar type, unsigned) const
{
    if (type == REGISTER_RGB)
        return CONTROL_RGBLED_SIZE;
    else if (type == REGISTER_EXTRA)
        return CONTROL_EXTRA_SIZE;
    else
        return CONTROL_JACK_SIZE;
}
