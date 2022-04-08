#ifndef PATCHSECTIONVIEW_H
#define PATCHSECTIONVIEW_H

#include "patchsection.h"

#include <QGraphicsView>

class PatchSectionView : public QGraphicsView
{
    PatchSection *section;

public:
    PatchSectionView(PatchSection *section);

private:
    void buildPatchSection();
};

#endif // PATCHSECTIONVIEW_H
