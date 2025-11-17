#ifndef PATCHSIZEINDICATOR_H
#define PATCHSIZEINDICATOR_H

#include <QTimer>
#include <QElapsedTimer>

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
    bool dirty;
    QElapsedTimer lastDirty;
    QTimer timer;

public:
    explicit PatchSizeIndicator(MainWindow *mainWindow, PatchEditEngine *patch);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private slots:
    void updateStatus();
    void checkDirty();

private:
    void recompute();
};

#endif // PATCHSIZEINDICATOR_H
