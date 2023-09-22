#include "modulee4.h"
#include "tuning.h"

unsigned ModuleE4::numRegisters(register_type_t type) const
{
    if (type == REGISTER_ENCODER || type == REGISTER_BUTTON || type == REGISTER_LED)
        return 4;
    else
        return 0;
}
QPointF ModuleE4::registerPosition(register_type_t, unsigned number) const
{
    float x = 3.0;
    float y = 5.490 * (number - 1) + 4.33;
    return QPointF(x, y);
}
float ModuleE4::registerSize(register_type_t type, unsigned) const
{
    if (type == REGISTER_ENCODER || type == REGISTER_BUTTON)
        return CONTROL_LARGE_POT_SIZE;
    else
        return 7.0;
}
float ModuleE4::labelDistance(register_type_t type, unsigned) const
{
    if (type == REGISTER_ENCODER)
        return -4.40;
    else
        return -3.68; // button
}
float ModuleE4::labelWidth(register_type_t, unsigned) const
{
    return 5.0;
}
float ModuleE4::rectAspect(register_type_t type) const
{
    if (type == REGISTER_LED)
        return 1.0;
    else
        return 0;
}
