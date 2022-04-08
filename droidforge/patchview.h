#ifndef PATCHVIEW_H
#define PATCHVIEW_H

#include "patch.h"

#include <QTabWidget>

class PatchView : public QTabWidget
{
    Patch *patch;

public:
    PatchView(Patch *patch);
};

#endif // PATCHVIEW_H
