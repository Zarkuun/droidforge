#ifndef MEMORYINDICATOR_H
#define MEMORYINDICATOR_H

#include "patcheditengine.h"
#include "patchview.h"

class MainWindow;

class MemoryIndicator : public QWidget, PatchView
{
    Q_OBJECT
    MainWindow *mainWindow;
    unsigned memoryNeeded;
    unsigned memoryAvailable;

public:
    explicit MemoryIndicator(MainWindow *mainWindow, PatchEditEngine *patch);
    void paintEvent(QPaintEvent *);

private slots:
    void updateStatus();

signals:

};

#endif // MEMORYINDICATOR_H
