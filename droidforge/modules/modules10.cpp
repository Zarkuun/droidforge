#include "modules10.h"
#include "tuning.h"

unsigned ModuleS10::numRegisters(register_type_t type) const
{
    if (type == REGISTER_SWITCH)
        return 10;
    else
        return 0;
}

QPointF ModuleS10::registerPosition(register_type_t, unsigned number) const
{
    if (number <= 2)
        return QPointF(hp()/2, 4.91 * (number - 1) + 3.50);
    else  {
        unsigned column = (number-3) % 2;
        unsigned row = (number-3) / 2;
        return QPointF(column * 2.36 + 1.32, row * 2.964 + 13.35);
    }
}

float ModuleS10::registerSize(register_type_t, unsigned number) const
{
   if (number <= 2)
       return CONTROL_LARGE_POT_SIZE;
   else
       return CONTROL_SMALL_SWITCH_SIZE;
}

float ModuleS10::labelDistance(register_type_t, unsigned number) const
{
    if (number <= 2)
        return 0.5;
    else
        return 0.47;
}

float ModuleS10::labelWidth(register_type_t, unsigned number) const
{
    if (number <= 2)
        return RACV_LARGE_LABEL_WIDTH;
    else
        return RACV_SMALL_LABEL_WIDTH;
}
