#include "moduleb32.h"
#include "tuning.h"

unsigned ModuleB32::numRegisters(register_type_t type) const
{
    if (type == REGISTER_BUTTON || type == REGISTER_LED)
        return 32;
    else
        return 0;
}
QPointF ModuleB32::registerPosition(register_type_t, unsigned number) const
{
   unsigned row = (number - 1) / 4;
   unsigned column = (number - 1) % 4;

   return QPointF(column * 2.50 + 1.258,
                  row * 2.767 + 2.99);
}
float ModuleB32::registerSize(register_type_t, unsigned) const
{
    return CONTROL_BUTTON_SIZE;
}
float ModuleB32::labelDistance(register_type_t, unsigned) const
{
    return -2.62;
}
