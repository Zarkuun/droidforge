#include "modulep10.h"
#include "tuning.h"

unsigned ModuleP10::numRegisters(QChar type) const
{
    if (type == REGISTER_POT)
        return 10;
    else
        return 0;
}

QPointF ModuleP10::registerPosition(QChar, unsigned number) const
{
    if (number <= 2)
        return QPointF(hp()/2, 4.91 * (number - 1) + 3.50);
    else  {
        unsigned column = (number-3) % 2;
        unsigned row = (number-3) / 2;
        return QPointF(column * 2.42 + 1.30, row * 2.94 + 12.93);
    }
}

float ModuleP10::registerSize(QChar, unsigned number) const
{
    if (number <= 2)
        return CONTROL_LARGE_POT_SIZE;
    else
        return CONTROL_SMALL_POT_SIZE;

}


