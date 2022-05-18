#include "modulep10.h"
#include "tuning.h"

unsigned ModuleP10::numControls(QChar type) const
{
    if (type == REGISTER_POT)
        return 10;
    else
        return 0;
}

QPointF ModuleP10::controlPosition(QChar, unsigned number) const
{
    if (number == 1)
        return QPointF(hp()/2, 3.63);
    else if (number == 2)
        return QPointF(hp()/2, 8.27);
    else  {
        unsigned column = (number-3) % 2;
        unsigned row = (number-3) / 2;
        return QPointF(column * 2.6 + 1.33, row * 2.98 + 13.0);
    }
}

float ModuleP10::controlSize(QChar, unsigned number) const
{
    if (number <= 2)
        return CONTROL_LARGE_POT_SIZE;
    else
        return CONTROL_SMALL_POT_SIZE;

}
