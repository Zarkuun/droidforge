#include "modulex7.h"
#include "tuning.h"

unsigned ModuleX7::numControls(QChar type) const
{
    if (type == 'G')
        return 4;
    else if (type == 'R')
        return 8;
    else
        return 0;
}

QPointF ModuleX7::controlPosition(QChar type, unsigned number)
{
    int column = (number - 1) % 2;
    int row = (number - 1) / 2;

    float x;
    float y;

    if (type == 'G') {
        x = column * 2.05 + 0.98;
        y = row * 2.55 + 19.6;
    }
    else { // 'R'
        y = row * 1.8 + 5.8;
        if (row >= 2)
            y += 0.3;
        x = column * 1.85 + 1.1;
    }
    return QPointF(x, y);

}

float ModuleX7::controlSize(QChar type, unsigned)
{
    if (type == 'R')
        return CONTROL_RGBLED_SIZE;
    else
        return CONTROL_JACK_SIZE;

}
