#include "modulemaster18.h"

unsigned ModuleMaster18::numRegisters(register_type_t type) const
{
    if (type == REGISTER_INPUT)
        return 2;
    else if (type == REGISTER_OUTPUT)
        return 8;
    else if (type == REGISTER_GATE)
        return 4;
    else
        return 0;
}

QPointF ModuleMaster18::registerPosition(register_type_t type, unsigned number) const
{
    int row;
    int column;

    if (type == REGISTER_INPUT) {
        row = number - 1;
        column = 0;
    }
    else if (type == REGISTER_GATE) {
        row = number + 1;
        column = 0;
    }
    else { // OUTPUT
        row = 2 + (number - 1) / 2;
        column = 1 + (number - 1) % 2;
    }

    float x = column * 2.00 + 0.98;
    float y = row * 2.55 + 9.37;
    return QPointF(x, y);
}

float ModuleMaster18::registerSize(register_type_t, unsigned) const
{
    return CONTROL_JACK_SIZE;
}

float ModuleMaster18::labelDistance(register_type_t, unsigned) const
{
    return -2.45;
}
