#include "moduleb32.h"
#include "tuning.h"

unsigned ModuleB32::numRegisters(QChar type) const
{
    if (type == REGISTER_BUTTON || type == REGISTER_LED)
        return 32;
    else
        return 0;
}

QPointF ModuleB32::registerPosition(QChar, unsigned number) const
{
   unsigned row = (number - 1) / 4;
   unsigned column = (number - 1) % 4;

   return QPointF(column * 2.50 + 1.258,
                  row * 2.767 + 2.99);
}

float ModuleB32::registerSize(QChar, unsigned) const
{
    return CONTROL_BUTTON_SIZE;
}
