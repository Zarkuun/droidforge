#include "modulep4b2.h"
#include "tuning.h"

unsigned ModuleP4B2::numControls(QChar type) const
{
    if (type == REGISTER_BUTTON || type == REGISTER_LED)
        return 2;
    else if (type == REGISTER_POT)
        return 4;
    else
        return 0;
}

QPointF ModuleP4B2::controlPosition(QChar type, unsigned number) const
{
    if (type == REGISTER_POT)
        return QPointF(hp()/2, 4.71 * (number - 1) + 3.63);
    else  {
        unsigned column = (number-1) % 2;
        return QPointF(column * 2.2 + 1.35, 8.85 + 13.4);
    }

}

float ModuleP4B2::controlSize(QChar type, unsigned) const
{
    if (type == REGISTER_POT)
        return CONTROL_LARGE_POT_SIZE;
    else
        return CONTROL_BUTTON_SIZE;
}
