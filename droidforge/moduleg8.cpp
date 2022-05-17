#include "moduleg8.h"
#include "tuning.h"

unsigned ModuleG8::numControls(QChar type) const
{
    if (type == 'G' || type == 'R')
        return 8;
    else
        return 0;
}

QPointF ModuleG8::controlPosition(QChar type, unsigned number)
{
    int column = (number - 1) % 2;
    int row = (number - 1) / 2;

    float x;
    float y;

    if (type == 'G') {
        y = row * 2.55 + 14.5;
        x = column * 2.05 + 0.9;
    }
    else { // 'R'
        y = row * 1.8 + 5.8;
        if (row >= 2)
            y += 0.3;
        x = column * 1.85 + 1.2;
    }
    return QPointF(x, y);
}

float ModuleG8::controlSize(QChar type, unsigned)
{
    if (type == 'R')
        return CONTROL_RGBLED_SIZE;
    else
        return CONTROL_JACK_SIZE;

}
