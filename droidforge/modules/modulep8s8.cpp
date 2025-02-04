#include "modulep8s8.h"
#include "tuning.h"

unsigned ModuleP8S8::numRegisters(register_type_t type) const
{
    if (type == REGISTER_LED)
        return 8;
    else if (type == REGISTER_POT)
        return 8;
    else if (type == REGISTER_SWITCH)
        return 8;
    else
        return 0;
}
bool ModuleP8S8::labelNeedsBackground(register_type_t, unsigned) const
{
    return true;
}
QPointF ModuleP8S8::labelPosition(register_type_t type, unsigned number) const
{
    if (type == REGISTER_POT) {
        float x = ((number-1) % 4) * 1.65 + 0.05;
        float y = 3.90 + ((number-1) % 4) * 0.75;
        if (number >= 5)
            y += 7.80;
        return QPointF(x, y);
    }
    else
        return QPointF(0.0, 0.0);
}
QPointF ModuleP8S8::registerPosition(register_type_t type, unsigned number) const
{
    float x = (((number-1) % 4) * 1.985 + 1.05) * (hp() / 8);
    float y;
    if (type == REGISTER_POT || type == REGISTER_LED)
        y = ((number-1) / 4) * 7.87 + 8.08;
    else // switch
        y = ((number-1) / 4) * 2.82 + 19.45;
    return QPointF(x, y);
}
float ModuleP8S8::registerSize(register_type_t type, unsigned) const
{
    if (type == REGISTER_SWITCH)
        return CONTROL_BUTTON_SIZE;
    else
        return 1.99; // return CONTROL_SLIDER_SIZE;
}
float ModuleP8S8::labelDistance(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return -4.00;
    else
        return -2.62;
}
float ModuleP8S8::labelWidth(register_type_t type, unsigned) const
{
    if (type == REGISTER_POT)
        return 3.00;
    else
        return 2.00; // RACV_BUTTON_LABEL_WIDTH;
}

float ModuleP8S8::rectAspect(register_type_t type, unsigned int) const
{
    if (type != REGISTER_POT)
        return 0.0;
    return 1.2;
}
