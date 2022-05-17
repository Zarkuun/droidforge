#include "modulep2b8.h"
#include "tuning.h"

unsigned ModuleP2B8::numControls(QChar type) const
{
    if (type == 'B' || type == 'L')
        return 8;
    else if (type == 'P')
        return 2;
    else
        return 0;
}


QPointF ModuleP2B8::controlPosition(QChar type, unsigned number)
{
    if (type == 'P') {
        if (number == 1)
            return QPointF(hp()/2, 3.63);
        else
            return QPointF(hp()/2, 8.27);
    }
    else  {
        unsigned column = (number-1) % 2;
        unsigned row = (number-1) / 2;
        return QPointF(column * 2.2 + 1.35, row * 2.95 + 13.4);
    }
}

float ModuleP2B8::controlSize(QChar type, unsigned)
{
    if (type == 'P')
        return CONTROL_LARGE_POT_SIZE;
    else
        return CONTROL_BUTTON_SIZE;
}
