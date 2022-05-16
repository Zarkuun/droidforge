#include "module.h"

QPixmap *Module::faceplateImage() const
{
    return new QPixmap(QString(":images/faceplates/" + faceplate()));
}
