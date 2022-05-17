#ifndef MODULEG8_H
#define MODULEG8_H

#include "module.h"

class ModuleG8 : public Module
{
public:
    ModuleG8() : Module("faceplate-g8-off") {}
    QString name() const { return "g8"; }
    QString faceplate() const { return "faceplate-g8-off"; }
    QString title() const { return "G8 Gates Expander"; }
    float hp() const { return 4; }
    unsigned numControls(QChar type) const;

protected:
    QPointF controlPosition(QChar type, unsigned number);;
    float controlSize(QChar type, unsigned number);
};

#endif // MODULEG8_H
