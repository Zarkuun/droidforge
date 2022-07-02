#include "modulep2b8.h"
#include "tuning.h"

unsigned ModuleP2B8::numRegisters(QChar type) const
{
    if (type == REGISTER_BUTTON || type == REGISTER_LED)
        return 8;
    else if (type == REGISTER_POT)
        return 2;
    else
        return 0;
}

bool ModuleP2B8::labelNeedsBackground(QChar type, unsigned) const
{
    return type == REGISTER_POT;
}


QPointF ModuleP2B8::registerPosition(QChar type, unsigned number) const
{
    if (type == REGISTER_POT) {
        return QPointF(hp()/2, 4.91 * (number - 1) + 3.50);
    }
    else  {
        unsigned column = (number-1) % 2;
        unsigned row = (number-1) / 2;
        return QPointF(column * 2.5 + 1.27, row * 2.97 + 13.43);
    }
}

float ModuleP2B8::registerSize(QChar type, unsigned) const
{
    if (type == REGISTER_POT)
        return CONTROL_LARGE_POT_SIZE;
    else
        return CONTROL_BUTTON_SIZE;
}
