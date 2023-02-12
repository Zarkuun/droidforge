#ifndef PATCHSIZEINDICATOR_H
#define PATCHSIZEINDICATOR_H

#include "patcheditengine.h"
#include "patchview.h"

class MainWindow;

class PatchSizeIndicator : public QWidget, PatchView
{
    Q_OBJECT
    MainWindow *mainWindow;
    unsigned memoryNeeded;
    unsigned memoryAvailable;
    unsigned patchSize; // counts to MAX_DROID_INI

public:
    explicit PatchSizeIndicator(MainWindow *mainWindow, PatchEditEngine *patch);
    void paintEvent(QPaintEvent *);

private slots:
    void updateStatus();

signals:

};

#endif // PATCHSIZEINDICATOR_H
