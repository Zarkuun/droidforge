#include "moduleb32.h"
#include "tuning.h"

unsigned ModuleB32::numControls(QChar type) const
{
    if (type == 'B' || type == 'L')
        return 32;
    else
        return 0;
}

QPointF ModuleB32::controlPosition(QChar, unsigned number)
{
   unsigned row = (number - 1) / 4;
   unsigned column = (number - 1) % 4;

   return QPointF(column * 2.55 + 1.20,
                  row * 2.75 + 3.0);

}

float ModuleB32::controlSize(QChar, unsigned)
{
    return CONTROL_BUTTON_SIZE;
}
