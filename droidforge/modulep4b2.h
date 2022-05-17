#ifndef MODULEP4B2_H
#define MODULEP4B2_H

#include "module.h"

class ModuleP4B2 : public Module
{
public:
    ModuleP4B2() : Module("faceplate-p4b2-off") { }
    QString name() const { return "p4b2"; }
    QString faceplate() const { return "faceplate-p4b2-off"; }
    QString title() const { return "P4B2 Controller"; }
    float hp() const { return 5; }
    unsigned numControls(QChar type) const;

protected:
    QPointF controlPosition(QChar type, unsigned number);;
    float controlSize(QChar type, unsigned number);
};

#endif // MODULEP4B2_H
