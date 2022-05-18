#ifndef MODULES10_H
#define MODULES10_H

#include "module.h"

class ModuleS10 : public Module
{
public:
    ModuleS10() : Module("faceplate-s10") {  }
    QString name() const { return "s10";  }
    QString faceplate() const  { return "faceplate-s10";  }
    QString title() const  { return "S10 Controller";  }
    float hp() const  { return 5; }
    unsigned numControls(QChar type) const;

protected:
    QPointF controlPosition(QChar type, unsigned number) const;
    float controlSize(QChar type, unsigned number) const;
};

#endif // MODULES10_H
