#include "modulep4b2.h"
#include "tuning.h"

unsigned ModuleP4B2::numRegisters(register_type_t type) const
{
    if (type == REGISTER_BUTTON || type == REGISTER_LED)
        return 2;
    else if (type == REGISTER_POT)
        return 4;
    else
        return 0;
}
bool ModuleP4B2::labelNeedsBackground(register_type_t type, unsigned) const
{
    return type == REGISTER_POT;
}
QPointF ModuleP4B2::registerPosition(register_type_t type, unsigned number) const
{
    if (type == REGISTER_POT)
        return QPointF(hp()/2, 4.91 * (number - 1) + 3.50);
    else  {
        unsigned column = (number-1) % 2;
        unsigned row = 3;
        return QPointF(column * 2.5 + 1.27, row * 2.97 + 13.43);
    }

}
float ModuleP4B2::registerSize(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return CONTROL_LARGE_POT_SIZE;
    else
        return CONTROL_BUTTON_SIZE;
}
float ModuleP4B2::labelDistance(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return 0.5;
    else
        return 0.17;
}

float ModuleP4B2::labelWidth(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return RACV_LARGE_LABEL_WIDTH;
    else
        return RACV_SMALL_LABEL_WIDTH;
}
