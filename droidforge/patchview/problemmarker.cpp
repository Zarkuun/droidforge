#include "problemmarker.h"
#include "tuning.h"

#include <QPainter>


QImage warningImage(":images/icons/warning.png"); // TODO central storage

ProblemMarker::ProblemMarker(unsigned size, QString toolTip)
    : IconMarker(warningImage, size, toolTip)
{
}
