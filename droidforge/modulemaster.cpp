#include "modulemaster.h"
#include "tuning.h"


unsigned ModuleMaster::numControls(QChar type) const
{
    if (type == 'I' || type == 'O' || type == 'N')
        return 8;
    else if (type == 'R')
        return 16;
    else if (type == 'X')
        return 1;
    else
        return 0;
}

QPointF ModuleMaster::controlPosition(QChar type, unsigned number)
{
    int column = (number - 1) % 4;
    int row = (number - 1) / 4;

    float y;
    float x = column * 2.05 + 0.9;

    if (type == 'I' || type == 'N')
        y = row * 2.55 + 14.5;
    else if (type == 'O')
        y = row * 2.55 + 19.6;
    else if (type == 'R') {
        y = row * 1.8 + 5.8;
        if (row >= 2)
            y += 0.3;
        x = column * 1.85 + 1.3;
    }
    else {
        return QPointF(4.0, 8.55);
    }
    return QPointF(x, y);
}

float ModuleMaster::controlSize(QChar type, unsigned number)
{
    if (type == 'R')
        return CONTROL_RGBLED_SIZE;
    else if (type == 'X')
        return CONTROL_EXTRA_SIZE;
    else
        return CONTROL_JACK_SIZE;
}
