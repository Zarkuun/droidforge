#include "moduledb8e.h"
#include "tuning.h"


unsigned ModuleDB8E::numRegisters(register_type_t type) const
{
    if (type == REGISTER_ENCODER)
        return 1;
    else if (type == REGISTER_BUTTON)
        return 9;
    else if (type == REGISTER_LED)
        return 9;
    else
        return 0;
}
QPointF ModuleDB8E::registerPosition(register_type_t type, unsigned number) const
{
    if (type == REGISTER_ENCODER || number == 9) {
        return QPointF(hp() / 2, 20.8);
    }
    else  {
        unsigned column = (number-1) % 2;
        unsigned row = (number-1) / 2;
        return QPointF(column * 2.65 + 1.68, row * 2.97 + 7.68);
    }
}
float ModuleDB8E::registerSize(register_type_t type, unsigned number) const
{
    if (type == REGISTER_ENCODER || (type == REGISTER_BUTTON && number == 9))
        return CONTROL_LARGE_POT_SIZE;
    else if (type == REGISTER_LED && number == 9)
        return CONTROL_ENCODER_LEDRING_SIZE;
    else if (type == REGISTER_BUTTON || type == REGISTER_LED)
        return CONTROL_BUTTON_SIZE;
    else
        return 20.0;
}
float ModuleDB8E::labelDistance(register_type_t type, unsigned) const
{
    if (type == REGISTER_ENCODER)
        return -4.40;
    else
        return -3.68; // button
}
float ModuleDB8E::labelWidth(register_type_t, unsigned) const
{
    return 5.0;
}
float ModuleDB8E::rectAspect(register_type_t type, unsigned number) const
{
    if (type == REGISTER_LED && number == 9)
        return 1.0;
    else
        return 0;
}
