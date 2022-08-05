#ifndef MEMORYINDICATOR_H
#define MEMORYINDICATOR_H

#include "patcheditengine.h"
#include "patchview.h"

class MemoryIndicator : public QWidget, PatchView
{
    Q_OBJECT

public:
    explicit MemoryIndicator(PatchEditEngine *patch, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);

private slots:
    void updateStatus();

signals:

};

#endif // MEMORYINDICATOR_H
