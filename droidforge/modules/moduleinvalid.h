#ifndef MODULEINVALID_H
#define MODULEINVALID_H

#include "module.h"

class ModuleInvalid : public Module
{
public:
    ModuleInvalid() : Module("faceplate-blind-front") {}
    QString name() const { return "invalid"; };
    QString faceplate() const { return "faceplate-blind-front"; };
    QString title() const { return "Invalid"; };
    float hp() const { return 1; };
    QPointF registerPosition(QChar, unsigned) const { return QPointF(0, 0); };
    float registerSize(QChar, unsigned) const { return 0.0; };
};

#endif // MODULEINVALID_H
