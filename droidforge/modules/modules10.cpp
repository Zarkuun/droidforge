#include "modules10.h"
#include "tuning.h"

unsigned ModuleS10::numRegisters(QChar type) const
{
    if (type == REGISTER_SWITCH)
        return 10;
    else
        return 0;
}

QPointF ModuleS10::registerPosition(QChar, unsigned number) const
{
    if (number == 1)
        return QPointF(hp()/2, 3.63);
    else if (number == 2)
        return QPointF(hp()/2, 8.27);
    else  {
        unsigned column = (number-3) % 2;
        unsigned row = (number-3) / 2;
        return QPointF(column * 2.5 + 1.25, row * 2.95 + 13.4);
    }
}

float ModuleS10::registerSize(QChar, unsigned number) const
{
   if (number <= 2)
       return CONTROL_LARGE_POT_SIZE;
   else
       return CONTROL_SMALL_SWITCH_SIZE;
}