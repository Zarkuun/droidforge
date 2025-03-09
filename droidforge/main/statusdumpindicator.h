#ifndef STATUSDUMPINDICATOR_H
#define STATUSDUMPINDICATOR_H

#include "patcheditengine.h"
#include "patchview.h"

class MainWindow;

class StatusDumpIndicator : public QWidget, PatchView
{
    Q_OBJECT
    MainWindow *mainWindow;
    QLabel *label;
    QToolButton *buttonPrev;
    QToolButton *buttonNext;
    QToolButton *buttonInfo;
    QToolButton *buttonClose;
    int dumpIndex;
    unsigned dumpCount;

public:
    explicit StatusDumpIndicator(MainWindow *mainWindow, PatchEditEngine *patch, QWidget *parent = nullptr);
    void updateStatus(unsigned numStatusDumps, int index);
    // void paintEvent(QPaintEvent *);

private slots:
    void switchPrev();
    void switchNext();
    void showDumpInfo();
    void hideDumps();
};

#endif // STATUSDUMPINDICATOR_H
