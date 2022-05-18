#ifndef MODULEMASTER_H
#define MODULEMASTER_H

#include "module.h"

class ModuleMaster : public Module
{
public:
    ModuleMaster() : Module("faceplate-master-off") {  }
    QString name() const { return "master"; }
    QString faceplate() const { return "faceplate-master-off";  }
    QString title() const { return "DROID master"; }
    float hp() const { return 8; }
    unsigned numControls(QChar type) const;

protected:
    QPointF controlPosition(QChar type, unsigned number) const;
    float controlSize(QChar type, unsigned number) const;
};

#endif // MODULEMASTER_H
